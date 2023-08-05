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

class ExternalFileLevelStorage : public LevelStorage, ChunkStorage
{
public:
	ExternalFileLevelStorage(const std::string& a, const std::string& path);
	~ExternalFileLevelStorage();

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
	static bool writeLevelData(const std::string& path, LevelData* pLevelData);

public:
	std::string field_8;
	std::string m_levelDirPath;
	LevelData* m_pLevelData = nullptr;
	RegionFile* m_pRegionFile = nullptr;
	Level* m_pLevel = nullptr;
	int m_timer = 0;
	std::list<UnsavedLevelChunk> m_unsavedLevelChunks;
};

#endif
