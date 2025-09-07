/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <list>
#include "LevelStorage.hpp"
#include "ChunkStorage.hpp"
#include "RegionFile.hpp"

#ifndef DEMO

struct UnsavedLevelChunk
{
	int m_index;
	int m_foundTime;
	LevelChunk* m_pChunk;
};

class ExternalFileLevelStorage : public LevelStorage, public ChunkStorage
{
public:
	ExternalFileLevelStorage(const std::string& name, const std::string& path, bool forceConversion = false);
	~ExternalFileLevelStorage();

private:
	void _setLevelData(LevelData* levelData);

public:
	// LevelStorage
	LevelData* prepareLevel(Level* level) override;
	ChunkStorage* createChunkStorage(Dimension*) override;
	void saveLevelData(const std::string& levelPath, LevelData* levelData, const std::vector<Player*>* players) override;
	void saveLevelData(LevelData* levelData, const std::vector<Player*>* players) override;
	void savePlayerData(LevelData& levelData, const std::vector<Player*>& players) override;
	void saveGame(Level* level) override;
	void closeAll() override;
	void tick() override;
	void flush() override;

	// ChunkStorage
	LevelChunk* load(Level* level, const ChunkPos& pos) override;
	void loadEntities(Level* level, LevelChunk* chunk) override;
	void save(Level* level, LevelChunk* chunk) override;
	void saveEntities(Level* level, LevelChunk* chunk) override;

	static bool readLevelData(const std::string& path, LevelData& levelData);
	static bool readPlayerData(const std::string& path, LevelData& levelData);
	static bool writeLevelData(const std::string& path, const LevelData& levelData, const std::vector<Player*>* players = nullptr);

public:
	std::string m_levelName;
	std::string m_levelDirPath;
	LevelData* m_pLevelData;
	RegionFile* m_pRegionFile;
	Level* m_pLevel;
	int m_timer;
	unsigned int m_storageVersion;
	std::list<UnsavedLevelChunk> m_unsavedLevelChunks;
	int m_lastEntitySave;
    bool m_bForceConversion;
};

#endif
