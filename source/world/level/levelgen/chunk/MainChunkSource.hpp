#pragma once

#include <map>

#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/level/levelgen/chunk/ChunkRefCount.hpp"

class MainChunkSource : public ChunkSource
{
protected:
	ChunkMap_t m_chunks;

public:
	MainChunkSource(std::unique_ptr<ChunkSource> parent);
	virtual ~MainChunkSource();

public:
	LevelChunk* getExistingChunk(const ChunkPos& pos) override;
	LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode) override;
	bool releaseChunk(LevelChunk& chunk) override;
	void acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk) override;
	const ChunkMap_t& getStoredChunks() override;
};
