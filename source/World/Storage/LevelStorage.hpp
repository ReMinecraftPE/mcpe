/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ChunkStorage.hpp"
#include "LevelData.hpp"

class Dimension;
class Player;

typedef void ProgressListener;

class LevelStorage
{
public:
	virtual ~LevelStorage();
	virtual LevelData* prepareLevel(Level*) = 0;
	virtual ChunkStorage* createChunkStorage(Dimension*) = 0;
	virtual void saveLevelData(LevelData* levelData, std::vector<Player*>& players) = 0;
	virtual void saveLevelData(LevelData* levelData);
	virtual void savePlayerData(LevelData* levelData, std::vector<Player*>& players);
	virtual void closeAll() = 0;
};

