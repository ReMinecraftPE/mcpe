#pragma once

#include "ChunkSource.hpp"
class Level;

class PerformanceTestChunkSource : public ChunkSource
{
public:
	LevelChunk* create(int x, int z) override;
	LevelChunk* getChunk(int x, int z) override;
	bool hasChunk(int x, int z) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, int, int) override;
	bool shouldSave() override;
	int tick() override;

public:
	Level* m_pLevel = nullptr;
};
