#include "world/level/levelgen/chunk/ChunkRefCount.hpp"

ChunkRefCount::ChunkRefCount()
	: m_refCount(0),
	  m_chunk(nullptr)
{
}

ChunkRefCount::ChunkRefCount(LevelChunk& chunk, int refCount)
	: m_refCount(refCount),
	  m_chunk(&chunk)
{
}

ChunkRefCount::ChunkRefCount(std::unique_ptr<LevelChunk>&& chunk, int refCount)
	: m_refCount(refCount),
	  m_chunk(chunk.get())
{
	m_chunk = nullptr;
}

ChunkRefCount::~ChunkRefCount()
{
	delete m_chunk;
}

LevelChunk* ChunkRefCount::get() const
{
	return m_chunk;
}

LevelChunk* ChunkRefCount::grab()
{
	m_refCount++;
	return get();
}

bool ChunkRefCount::release()
{
	return --m_refCount != 0;
}

void ChunkRefCount::_move(ChunkRefCount& other)
{
	delete m_chunk;
	m_chunk = other.m_chunk;
	m_refCount = other.m_refCount;
	other.m_chunk = nullptr;
	other.m_refCount = 0;
}
