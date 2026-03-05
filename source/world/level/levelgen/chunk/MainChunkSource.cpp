#include "world/level/levelgen/chunk/MainChunkSource.hpp"

MainChunkSource::MainChunkSource(std::unique_ptr<ChunkSource> parent)
	: ChunkSource(std::move(parent))
{
}

MainChunkSource::~MainChunkSource()
{
}

LevelChunk* MainChunkSource::getExistingChunk(const ChunkPos& pos)
{
	ChunkMap_t::const_iterator iter = m_chunks.find(pos);
	if (iter != m_chunks.end())
		return iter->second.get();

	return nullptr;
}

LevelChunk* MainChunkSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	ChunkMap_t::iterator iter = m_chunks.find(pos);
	if (iter != m_chunks.end())
		return iter->second.grab();

	LevelChunk* chunk = m_parent->requestChunk(pos, loadMode);
	if (chunk)
		m_chunks[pos] = ChunkRefCount(*chunk, 1);
	return chunk;
}

bool MainChunkSource::releaseChunk(LevelChunk& chunk)
{
	ChunkMap_t::iterator iter = m_chunks.find(chunk.getPos());
	if (iter == m_chunks.end())
		return false;

	if (iter->second.release())
		return false;

	m_chunks.erase(iter);
	return true;
}

void MainChunkSource::acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk)
{
	ChunkMap_t::iterator iter = m_chunks.find(chunk->getPos());
	if (iter != m_chunks.end())
	{
		iter->second.grab();
		chunk.release();
	}
	else
	{
		m_chunks[chunk->getPos()] = ChunkRefCount(*chunk, 1);
	}
}

const ChunkSource::ChunkMap_t& MainChunkSource::getStoredChunks()
{
	return m_chunks;
}
