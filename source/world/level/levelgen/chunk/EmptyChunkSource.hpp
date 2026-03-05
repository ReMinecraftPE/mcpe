#pragma once
#include "world/level/levelgen/chunk/ChunkSource.hpp"

class EmptyChunkSource : public ChunkSource
{
public:
	EmptyChunkSource(Level* level, Dimension* dimension);

public:
	LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode) override;
	bool postProcess(ChunkViewSource& chunkViewSource) override;
	void loadChunk(LevelChunk& chunk) override;
};
