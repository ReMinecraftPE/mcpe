/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelStorage.hpp"

LevelStorage::~LevelStorage()
{
}

void LevelStorage::saveLevelData(LevelData* levelData)
{
	saveLevelData(levelData, nullptr);
}

void LevelStorage::savePlayerData(LevelData& levelData, const std::vector<Player*>& players)
{
}

void LevelStorage::saveGame(Level* level)
{
}

void LevelStorage::loadEntities(Level* level, LevelChunk* chunk)
{
}