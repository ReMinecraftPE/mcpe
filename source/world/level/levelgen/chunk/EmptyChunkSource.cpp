#include "world/level/levelgen/chunk/EmptyChunkSource.hpp"

EmptyChunkSource::EmptyChunkSource(Level* level, Dimension* dimension)
	: ChunkSource(level, dimension, 16)
{
}

LevelChunk* EmptyChunkSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	if (loadMode == Deferred)
		return new LevelChunk(*m_level, *m_dimension, pos, false);

	return nullptr;
}

bool EmptyChunkSource::postProcess(ChunkViewSource& chunkViewSource)
{
	return true;
}

void EmptyChunkSource::loadChunk(LevelChunk& chunk)
{
	memset(chunk.getTiles(), TILE_AIR, ChunkConstants::TILE_COUNT);
	chunk.setFinalized(LevelChunk::CF_DONE);
	chunk.setSaved();
	chunk.changeState(CS_GENERATING, CS_POST_PROCESSED);
}
