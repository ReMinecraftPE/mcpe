#pragma once

#include <list>
#include "LevelStorage.hpp"
#include "ChunkStorage.hpp"
#include "RegionFile.hpp"

#ifndef DEMO

class ExternalFileLevelStorage : public LevelStorage, ChunkStorage
{
public:
	ExternalFileLevelStorage(const std::string& a, const std::string& path);

	// LevelStorage
	LevelData* prepareLevel(Level* level) override;
	ChunkStorage* createChunkStorage(Dimension*) override;
	void saveLevelData(LevelData* levelData, std::vector<Player*>& players) override;
	void savePlayerData(LevelData* levelData, std::vector<Player*>& players) override;
	void closeAll() override;
	void tick() override;
	void flush() override;

	// ChunkStorage
	LevelChunk* load(Level* level, int x, int z) override;
	void save(Level* level, LevelChunk* chunk) override;
	void saveEntities(Level* level, LevelChunk* chunk) override;

	static bool readLevelData(const std::string& path, LevelData* pLevelData);
	static bool readPlayerData(const std::string& path, LevelData* pLevelData);

public:
	std::string field_8;
	std::string m_levelDirPath;
	LevelData* m_pLevelData = nullptr;
	RegionFile* m_pRegionFile = nullptr;
	Level* m_pLevel = nullptr;
	std::list<UnsavedLevelChunk> m_unsavedLevelChunks;
};

#endif
