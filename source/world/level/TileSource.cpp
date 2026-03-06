#include "world/level/TileSource.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSourceListener.hpp"

TileSource::TileSource(Level& level, Dimension& dimension, ChunkSource& source, bool publicSource, bool allowUnpopulatedChunks)
	: m_threadId(std::this_thread::get_id())
	, m_bAllowUnpopulatedChunks(allowUnpopulatedChunks)
	, m_bPublicSource(publicSource)
	, m_level(level)
	, m_chunkSource(source)
	, m_dimension(dimension)
	, m_lastChunk(nullptr)
	, m_tileTickingQueue(nullptr)
{
	if (publicSource)
	{
		m_listeners.push_back(&level);

		for (size_t i = 0; i < m_listeners.size(); i++)
		{
			m_listeners[i]->onSourceCreated(*this);
		}
	}
}

Level& TileSource::getLevel() const
{
	return m_level;
}

const Level& TileSource::getLevelConst() const
{
	return m_level;
}

Dimension& TileSource::getDimension() const
{
	return m_dimension;
}

const Dimension& TileSource::getDimensionConst() const
{
	return m_dimension;
}

void TileSource::setTickingQueue(TileTickingQueue& queue)
{
	m_tileTickingQueue = &queue;
}

TileTickingQueue* TileSource::getTickQueue(const TilePos& pos)
{
	if (m_tileTickingQueue)
		return m_tileTickingQueue;

	LevelChunk* chunk = getChunk(pos);
	if (chunk)
		return &chunk->getTileTickingQueue();

	return nullptr;
}

LevelChunk* TileSource::getChunk(const ChunkPos& pos)
{
	if (!m_lastChunk || m_lastChunk->getPos() != pos)
	{
		LevelChunk* chunk = m_bAllowUnpopulatedChunks ? m_chunkSource.getGeneratedChunk(pos) : m_chunkSource.getAvailableChunk(pos);
		if (chunk)
			m_lastChunk = chunk;
		return chunk;
	}

	return m_lastChunk;
}

LevelChunk* TileSource::getChunk(int x, int z)
{
	return getChunk(ChunkPos(x, z));
}

LevelChunk* TileSource::getWritableChunk(const ChunkPos& pos)
{
	LevelChunk* chunk = getChunk(pos);
	if (chunk)
	{
		if (!chunk->isReadOnly())
			return chunk;
	}

	return nullptr;
}

bool TileSource::shouldFireEvents(LevelChunk& chunk) const
{
	if (m_bAllowUnpopulatedChunks)
		return false;

	return chunk.getState() == CS_POST_PROCESSED;
}

bool TileSource::hasChunksAt(const Bounds& bounds)
{
	const Vec3Int32& min = bounds.getMin();
	const Vec3Int32& max = bounds.getMax();
	ChunkPos pos;
	for (pos.z = min.z; pos.z <= max.z; pos.z++)
	{
		for (pos.x = min.x; pos.x <= max.x; pos.x++)
		{
			LevelChunk* chunk = getChunk(pos);
			if (chunk)
				return true;
		}
	}

	return false;
}

bool TileSource::hasChunksAt(const AABB& aabb)
{
	return hasChunksAt(aabb.min * Vec3(1, 0, 1), aabb.max * Vec3(1, 0, 1));
}

bool TileSource::hasChunksAt(const TilePos& min, const TilePos& max)
{
	return hasChunksAt(Bounds(min, max, m_chunkSource.getChunkSide(), false));
}

bool TileSource::hasChunksAt(int x, int y, int z, int r)
{
	TilePos min(x - r, y, z - r);
	TilePos max(x + r, y, z + r);

	return hasChunksAt(min, max);
}

bool TileSource::hasChunksAt(const TilePos& pos, int r)
{
	return hasChunksAt(pos.x, pos.y, pos.z, r);
}

const std::vector<MobSpawnerData>& TileSource::getMobsAt(EntityType entityType, const TilePos& pos)
{
	return m_chunkSource.getMobsAt(*this, entityType, pos);
}

bool TileSource::hasNeighborSignal(int x, int y, int z)
{
	return false;
}

void TileSource::runLightUpdates(const LightLayer& lightLayer, const TilePos& min, const TilePos& max)
{
	// TODO!!
}

void TileSource::onChunkDiscarded(LevelChunk& chunk)
{
	if (m_lastChunk == &chunk)
		clearCachedLastChunk();
}

void TileSource::clearCachedLastChunk()
{
	m_lastChunk = nullptr;
}

bool TileSource::isOwnerThread() const
{
	return m_threadId == std::this_thread::get_id();
}

TileID TileSource::getTile(const TilePos& pos)
{
	if (pos.y >= ChunkConstants::Y_SIZE)
		return TILE_AIR;

	LevelChunk* chunk = getChunk(pos);
	if (!chunk)
		return TILE_AIR;

	return chunk->getTile(pos);
}

TileID TileSource::getTile(int x, int y, int z)
{
	return getTile(TilePos(x, y, z));
}

Tile* TileSource::getTilePtr(const TilePos& pos)
{
	return Tile::tiles[getTile(pos)];
}

Tile* TileSource::getTilePtr(int x, int y, int z)
{
	return getTilePtr(TilePos(x, y, z));
}

TileData TileSource::getData(const TilePos& pos)
{
	if (pos.y >= ChunkConstants::Y_SIZE)
		return 0;

	LevelChunk* chunk = getChunk(pos);
	if (!chunk)
		return 0;

	return chunk->getData(pos);
}

TileData TileSource::getData(int x, int y, int z)
{
	return getData(TilePos(x, y, z));
}

Brightness_t TileSource::getRawBrightness(const TilePos& pos, bool unk)
{
	TileID tile = getTile(pos);

	if (Tile::lightBlock[tile] == Brightness::MAX)
		return Brightness::MIN;

	if (unk)
	{
		// TODO: also handle wood slabs
		if (*Tile::stoneSlabHalf == tile || *Tile::farmland == tile)
		{
			Brightness_t brightestNeighbor = 0;
			for (int i = 0; i < Facing::COUNT; i++)
			{
				Brightness_t neighborBrightness = getRawBrightness(pos + Facing::DIRECTION[i], false);
				if (neighborBrightness > brightestNeighbor)
					brightestNeighbor = neighborBrightness;
			}
			return brightestNeighbor;
		}
	}

	if (pos.y < 0)
		return Brightness::MIN;

	if (pos.y > (ChunkConstants::Y_SIZE - 1))
	{
		Brightness_t skyDarken = getSkyDarken();
		Brightness_t b = Brightness::MAX - skyDarken;
		if (b > Brightness::MAX)
			b = Brightness::MIN; // bug?
		return b;
	}

	LevelChunk* chunk = getChunk(pos);
	if (!chunk)
		return Brightness::MAX;

	return chunk->getRawBrightness(pos, getSkyDarken());
}

Brightness_t TileSource::getRawBrightness(int x, int y, int z, bool unk)
{
	return getRawBrightness(TilePos(x, y, z), unk);
}

Brightness_t TileSource::getBrightness(const LightLayer& lightLayer, const TilePos& pos)
{
	if (pos.y > (ChunkConstants::Y_SIZE - 1))
		return lightLayer.getSurrounding();

	LevelChunk* chunk = getChunk(pos);
	if (!chunk)
		return Brightness::MIN;

	return chunk->getBrightness(lightLayer, pos);
}

Brightness_t TileSource::getBrightness(const LightLayer& lightLayer, int x, int y, int z)
{
	return getBrightness(lightLayer, TilePos(x, y, z));
}

float TileSource::getBrightness(const TilePos& pos)
{
	return m_dimension.getBrightnessRamp(getRawBrightness(pos, true));
}

float TileSource::getBrightness(int x, int y, int z)
{
	return getBrightness(TilePos(x, y, z));
}

void TileSource::_neighborChanged(const TilePos& neighborPos, const TilePos& originalPos, TileID tileID)
{
	if (getLevelConst().m_bIsClientSide)
		return;

	// TODO
}

void TileSource::_tileChanged(const TilePos& pos, FullTile oldTile, FullTile newTile, TileChange updateFlags)
{
	if (updateFlags.isUpdateNeighbors())
	{
		updateNeighborsAt(pos, oldTile.getTypeId());
	}

	if (updateFlags.isUpdateListeners())
	{
		if (!m_level.m_bIsClientSide || !updateFlags.isUpdateListenersServerOnly())
		{
			fireTileChanged(pos, oldTile, newTile, updateFlags);
		}
	}
}
