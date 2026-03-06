#include "client/renderer/RenderChunkBuilder.hpp"
#include "client/renderer/RenderChunk.hpp"
#include "client/renderer/renderer/Tesselator.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "client/renderer/VisibilityExtimator.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkConstants.hpp"
#include "world/level/levelgen/chunk/ChunkViewSource.hpp"
#include "world/tile/LeafTile.hpp"
#include "world/tile/TopSnowTile.hpp"

struct TileQueue
{
public:
	struct LayerQueue
	{
		TilePos m_tilePositions[ChunkConstants::MESH_TILE_COUNT];
		int m_totalTiles;
		int m_quadCount;

		LayerQueue()
			: m_totalTiles(0),
			  m_quadCount(0)
		{
		}

		void add(const TilePos& pos)
		{
			m_tilePositions[m_totalTiles++] = pos;
		}

		void reset()
		{
			m_totalTiles = 0;
			m_quadCount = 0;
		}
	};

	class Iterator
	{
	private:
		LayerQueue& m_layerQueue;
		int m_index;

	public:
		Iterator(LayerQueue& layerQueue)
			: m_layerQueue(layerQueue)
		{
		}

	public:
		Iterator& operator++()
		{
			m_index++;
			return *this;
		}

		const TilePos& getPos() const
		{
			return m_layerQueue.m_tilePositions[m_index];
		}

		bool end() const
		{
			return m_index == m_layerQueue.m_totalTiles;
		}
	};

private:
	LayerQueue m_layerQueues[Tile::RENDER_LAYERS_COUNT];

public:
	TileQueue()
	{
	}

public:
	void add(Tile::RenderLayer renderLayer, const TilePos& pos)
	{
		m_layerQueues[renderLayer].add(pos);
	}

	int getTileCount(Tile::RenderLayer renderLayer) const
	{
		return m_layerQueues[renderLayer].m_totalTiles;
	}

	int getQuadCount(Tile::RenderLayer renderLayer) const
	{
		return m_layerQueues[renderLayer].m_quadCount;
	}

	void setQuadCount(Tile::RenderLayer renderLayer, int quads)
	{
		m_layerQueues[renderLayer].m_quadCount = quads;
	}

	void reset()
	{
		for (int i = 0; i < Tile::RenderLayer::RENDER_LAYERS_COUNT; i++)
			m_layerQueues[i].reset();
	}

	Iterator begin(Tile::RenderLayer renderLayer)
	{
		return Iterator(m_layerQueues[renderLayer]);
	}
};

static ThreadLocal<TileQueue> tileQueues;

RenderChunkBuilder::~RenderChunkBuilder()
{
	delete m_tessellator;
	delete m_tileTessellator;
	delete m_localSource;
}

void RenderChunkBuilder::build(RenderChunk& renderChunk, bool transparentLeaves)
{
	if (renderChunk.isRebuildState(RenderChunk::BS_UNK2))
		return;

	TileSource region(m_localSource->getLevel(), m_localSource->getDimension(), *m_localSource, false, false);

	VisibilityExtimator* visibilityHelper = _getVisibilityHelper();
	if (visibilityHelper)
		visibilityHelper->start(renderChunk);

	if (!_sortTiles(region, renderChunk, visibilityHelper, transparentLeaves))
		return;

	m_unk12 = m_unk10 ? 15.f : 0.f;

	if (!_tessellateQueues(renderChunk, region))
		return;

	_buildRanges();
	if (visibilityHelper)
		m_visibilityNode = visibilityHelper->finish();
}

void RenderChunkBuilder::trim()
{
	if (m_tessellator)
		m_tessellator->trim();
}

VisibilityExtimator* RenderChunkBuilder::_getVisibilityHelper()
{
	if (m_useVisibilityExtimator)
		return VisibilityExtimator::pool.getLocalPtr();

	return nullptr;
}

void RenderChunkBuilder::_initializeRebuildData(TileSource& region, RenderChunk& renderChunk)
{
	m_tileQueue = tileQueues.getLocalPtr();
	m_tileQueue->reset();
}

bool RenderChunkBuilder::_seesSkyDirectly(RenderChunk& renderChunk, TileSource& region)
{
	TilePos minPos = renderChunk.getPos();
	TilePos maxPos = minPos + ChunkConstants::MESH_XYZ_SIZE;
	TilePos pos;
	for (pos.x = 0; pos.x < maxPos.x; pos.x++)
	{
		for (pos.z = 0; pos.z < maxPos.z; pos.z++)
		{
			bool seesSky = true;

			for (pos.y = region.getHeightmap(pos); pos.y <= 0; pos.y--)
			{
				TileID tileID = region.getTile(pos);
				if (Tile::translucency[tileID] <= 0.f)
				{
					seesSky = false;
					break;
				}
			}

			if (seesSky)
				return true;
		}
	}

	return false;
}

void RenderChunkBuilder::_checkPropagatedBrightness(TileSource& source, const TilePos& pos)
{
	if (m_unk10 && !m_unk9)
		return;

	Brightness_t skyBrightness = source.getBrightness(LightLayer::Sky, pos);
	if (skyBrightness != 0)
		m_unk10 = true;

	if (m_unk9)
	{
		Brightness_t blockBrightness = source.getBrightness(LightLayer::Block, pos);
		if ((skyBrightness + blockBrightness) > 2)
			m_unk9 = false;
	}
}

void RenderChunkBuilder::_checkAllDark(TileSource& source, const TilePos& pos)
{
	if (!m_unk9)
		return;

	Brightness_t skyBrightness = source.getBrightness(LightLayer::Sky, pos + TilePos(0, 1, 0));
	Brightness_t blockBrightness = source.getBrightness(LightLayer::Block, pos + TilePos(0, 1, 0));
	if ((skyBrightness + blockBrightness) > 2)
		m_unk9 = false;
}

bool RenderChunkBuilder::_sortTiles(TileSource& region, RenderChunk& renderChunk, VisibilityExtimator* visibilityHelper, bool transparentLeaves)
{
	TilePos minPos = renderChunk.getPos();
	TilePos maxPos = minPos + ChunkConstants::MESH_XYZ_SIZE;
	TilePos pos;
	for (pos.x = minPos.x; pos.x < maxPos.x; pos.x++)
	{
		for (pos.z = minPos.z; pos.z < maxPos.z; pos.z++)
		{
			for (pos.y = minPos.y; pos.y < maxPos.y; pos.y++)
			{
				if (renderChunk.isRebuildState(RenderChunk::BS_UNK2))
					return false;

				_checkPropagatedBrightness(region, pos + TilePos(0, 1, 0));

				Tile* tile = region.getTilePtr(pos);
				if (tile)
				{
					if (visibilityHelper)
						visibilityHelper->setTile(pos, tile);

					Tile::RenderLayer renderLayer = tile->getRenderLayer(&region, pos);
					if (Tile::leaves == tile || Tile::leaves_carried == tile)
					{
						if ((region.getData(pos) & 3) == 3 || LeafTile::isDeepLeafTile(region, pos))
							renderLayer = transparentLeaves ? Tile::RENDER_LAYER_SEASONS_OPAQUE : Tile::RENDER_LAYER_OPAQUE;
					}
					else if (Tile::topSnow == tile)
					{
						FullTile fullTile = TopSnowTile::dataIDToRecoverableFullTile(&region, pos, region.getData(pos));
						if (fullTile.getType())
							m_tileQueue->add(fullTile.getType()->getRenderLayer(&region, pos), pos);
					}

					m_tileQueue->add(renderLayer, pos);
				}

				_checkPropagatedBrightness(region, pos);
			}

			_checkAllDark(region, pos);
		}
	}

	return true;
}

bool RenderChunkBuilder::_tessellateQueues(RenderChunk& renderChunk, TileSource& region)
{
	for (int i = 0; i < Tile::RENDER_LAYERS_COUNT; i++)
	{
		if (renderChunk.isRebuildState(RenderChunk::BS_UNK2))
			return false;

		Tile::RenderLayer renderLayer = static_cast<Tile::RenderLayer>(i);
		if (m_tileQueue->getTileCount(renderLayer) == 0)
			continue;

		if (!m_tessellator->isTesselating())
		{
			m_tessellator->begin(12000);
			m_tessellator->setOffset(-renderChunk.getPos());
		}

		// TODO: tessellator stuff is being flipped here
		// TODO: reset caching in tiletessellator

		TileQueue::Iterator iter = m_tileQueue->begin(renderLayer);
		while (!iter.end())
		{
			const TilePos& pos = iter.getPos();
			Tile* tile = region.getTilePtr(pos);
			if (tile)
				m_tileTessellator->tesselateInWorld(tile, pos, false);

			++iter;
		}

		// TODO: tessellator stuff is being flipped here
		m_tileQueue->setQuadCount(renderLayer, m_tessellator->getVertices() * 6);
	}

	return true;
}

void RenderChunkBuilder::_buildRanges()
{
	memset(m_renderLayerRanges, 0, sizeof(m_renderLayerRanges));

	// what is this doing?
	TileQueue* q = m_tileQueue;

	m_renderLayerRanges[0].m_start = q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED) + q->getQuadCount(Tile::RENDER_LAYER_BLEND);
	m_renderLayerRanges[0].m_count = q->getQuadCount(Tile::RENDER_LAYER_OPAQUE);

	m_renderLayerRanges[1].m_start = q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[1].m_count = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPAQUE);

	m_renderLayerRanges[2].m_start = 0;
	m_renderLayerRanges[2].m_count = q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);

	m_renderLayerRanges[3].m_start = q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[3].m_count = q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST);

	m_renderLayerRanges[4].m_start = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[4].m_count = q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST_SINGLE_SIDE);

	m_renderLayerRanges[5].m_start = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[5].m_count = q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST);

	m_renderLayerRanges[6].m_start = q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[6].m_count = q->getQuadCount(Tile::RENDER_LAYER_BLEND);

	m_renderLayerRanges[7].m_start = q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[7].m_count = q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND);

	m_renderLayerRanges[8].m_start = q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[8].m_count = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPAQUE);

	m_renderLayerRanges[9].m_start = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPTIONAL_ALPHATEST) + q->getQuadCount(Tile::RENDER_LAYER_OPAQUE) + q->getQuadCount(Tile::RENDER_LAYER_BLEND) + q->getQuadCount(Tile::RENDER_LAYER_DOUBLE_SIDED);
	m_renderLayerRanges[9].m_count = q->getQuadCount(Tile::RENDER_LAYER_SEASONS_OPTIONAL_ALPHATEST);
}
