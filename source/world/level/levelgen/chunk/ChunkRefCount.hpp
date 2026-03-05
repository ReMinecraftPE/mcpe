#pragma once
#include <memory>
#include "compat/LegacyCPP_Compat.hpp"
#include "common/stl/unique_ptr.hpp"

class LevelChunk;

class ChunkRefCount
{
private:
	int m_refCount;
	LevelChunk* m_chunk;

public:
	ChunkRefCount();
	ChunkRefCount(LevelChunk& chunk, int refCount);
	ChunkRefCount(std::unique_ptr<LevelChunk>&& chunk, int refCount);
	MC_CTOR_MOVE(ChunkRefCount);
	~ChunkRefCount();

public:
	MC_FUNC_MOVE(ChunkRefCount);
	LevelChunk* get() const;
	LevelChunk* grab();
	bool release();

private:
	void _move(ChunkRefCount& other);
};
