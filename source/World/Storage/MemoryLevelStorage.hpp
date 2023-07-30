#pragma once

#include "LevelStorage.hpp"

class MemoryLevelStorage : public LevelStorage
{
public:
	LevelData* prepareLevel(Level*) override;
	ChunkStorage* createChunkStorage(Dimension*) override;
	void saveLevelData(LevelData* levelData, std::vector<Player*>& players) override;
	void saveLevelData(LevelData* levelData) override;
	void closeAll() override;
};

