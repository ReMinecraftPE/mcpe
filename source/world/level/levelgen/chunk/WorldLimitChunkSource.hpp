#pragma once
#include "world/level/levelgen/chunk/ChunkSource.hpp"

class WorldLimitChunkSource : public ChunkSource
{
protected:
	Bounds m_bounds;
	std::vector<LevelChunk*> m_chunks;
	LevelChunk* m_outOfBoundsChunk;

public:
	WorldLimitChunkSource(std::unique_ptr<ChunkSource> parent, const TilePos& limitOrigin);
	virtual ~WorldLimitChunkSource();

public:
	LevelChunk* getExistingChunk(const ChunkPos& pos) override;
	LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode) override;
	bool releaseChunk(LevelChunk& chunk) override;
};
