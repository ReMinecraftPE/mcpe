#include "world/level/levelgen/chunk/NetworkChunkSource.hpp"

LevelChunk* NetworkChunkSource::getIncomingChunk(const ChunkPos& pos)
{
	LevelChunk* existingChunk = getExistingChunk(pos);
	if (existingChunk)
		return existingChunk;

	LevelChunk* blankChunk = new LevelChunk(*m_level, *m_dimension, pos, false);
	m_incomingChunks[pos] = std::make_unique<LevelChunk>(blankChunk);

	return blankChunk;
}

LevelChunk* NetworkChunkSource::getExistingChunk(const ChunkPos& pos)
{
	ChunkMap_t::const_iterator iter = m_chunks.find(pos);
	if (iter != m_chunks.end())
		return iter->second.get();

	return nullptr;
}

LevelChunk* NetworkChunkSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	{
		ChunkMap_t::iterator iter = m_chunks.find(pos);
		if (iter != m_chunks.end())
			return iter->second.grab();
	}

	{
		IncomingChunkMap_t::iterator iter = m_incomingChunks.find(pos);
		if (iter != m_incomingChunks.end())
		{
			LevelChunk* chunk = iter->second.release();
			m_chunks[iter->first] = ChunkRefCount(*chunk, 1);
			m_incomingChunks.erase(iter);
			return chunk;
		}
	}

	if (loadMode == LOAD_DEFERRED)
	{
		LevelChunk* chunk = new LevelChunk(*m_level, *m_dimension, pos, false);
		m_chunks[pos] = ChunkRefCount(*chunk, 1);
		return chunk;
	}
}

bool NetworkChunkSource::releaseChunk(LevelChunk& chunk)
{
	ChunkMap_t::iterator iter = m_chunks.find(chunk.getPos());
	if (iter == m_chunks.end())
		return false;

	if (iter->second.release())
		return false;

	m_chunks.erase(iter);
	return true;
}

void NetworkChunkSource::acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk)
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

const ChunkSource::ChunkMap_t& NetworkChunkSource::getStoredChunks()
{
	return m_chunks;
}
