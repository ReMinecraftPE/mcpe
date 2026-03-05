#include "world/level/levelgen/chunk/WorldLimitChunkSource.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/tile/Tile.hpp"
#include "world/level/LevelConstants.hpp"

static Bounds GetWorldSizeBounds(const TilePos& limitOrigin)
{
	ChunkPos minPos(limitOrigin);
	minPos -= (LevelConstants::WORLD_SIZE_XZ / 2);
	ChunkPos maxPos(limitOrigin);
	maxPos += (LevelConstants::WORLD_SIZE_XZ / 2) - 1;

	return Bounds(minPos, maxPos, 16, true);
}

WorldLimitChunkSource::WorldLimitChunkSource(std::unique_ptr<ChunkSource> parent, const TilePos& limitOrigin)
	: ChunkSource(std::move(parent)),
	  m_bounds(GetWorldSizeBounds(limitOrigin))
{
	m_outOfBoundsChunk = new LevelChunk(*m_level, *m_dimension, ChunkPos::INVALID, false);
	memset(m_outOfBoundsChunk->getTiles(), Tile::invisible_bedrock->m_ID, ChunkConstants::TILE_COUNT);
	m_outOfBoundsChunk->changeState(CS_UNLOADED, CS_POST_PROCESSED);

	// load all chunks for fun
	Bounds::Iterator iter = m_bounds.begin();
	while (!iter.end())
	{
		ChunkPos pos(iter.pos().x, iter.pos().z);
		m_chunks.push_back(requestChunk(pos, LOAD_DEFERRED));
	}
}

LevelChunk* WorldLimitChunkSource::getExistingChunk(const ChunkPos& pos)
{
	if (m_bounds.contains(Vec3Int32(pos.x, 0, pos.z)))
		return m_parent->getExistingChunk(pos);
	else
		return m_outOfBoundsChunk;
}

LevelChunk* WorldLimitChunkSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	if (m_bounds.contains(Vec3Int32(pos.x, 0, pos.z)))
		return m_parent->requestChunk(pos, loadMode);
	else
		return m_outOfBoundsChunk;
}

bool WorldLimitChunkSource::releaseChunk(LevelChunk& chunk)
{
	if (&chunk == m_outOfBoundsChunk)
		return false;

	return m_parent->releaseChunk(chunk);
}
