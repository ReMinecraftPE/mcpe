#pragma once
#include "world/level/levelgen/chunk/ChunkSource.hpp"

class NetworkChunkSource : public ChunkSource
{
protected:
	typedef std::unordered_map<ChunkPos, std::unique_ptr<LevelChunk>> IncomingChunkMap_t;

protected:
	IncomingChunkMap_t m_incomingChunks;
	ChunkMap_t m_chunks;

public:
	NetworkChunkSource(Level* level, Dimension* dimension);
	virtual ~NetworkChunkSource();

public:
	LevelChunk* getIncomingChunk(const ChunkPos&);
	LevelChunk* getExistingChunk(const ChunkPos& pos) override;
	LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode) override;
	bool releaseChunk(LevelChunk& chunk) override;
	void acquireDiscarded(std::unique_ptr<LevelChunk>&& chunk) override;
	const ChunkMap_t& getStoredChunks() override;
};
