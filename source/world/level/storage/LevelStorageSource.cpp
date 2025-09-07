/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelStorageSource.hpp"

LevelStorageSource::~LevelStorageSource()
{
}

void LevelStorageSource::getLevelList(std::vector<LevelSummary>& vec)
{
	// @TODO: complete mock
#ifndef ORIGINAL_CODE
	vec.push_back(LevelSummary("Level1", "Level-1", 12345, 1234567, GAME_TYPE_CREATIVE, LEVEL_STORAGE_VERSION_DEFAULT));
	vec.push_back(LevelSummary("Level2", "Level-2", 23456, 2345678, GAME_TYPE_CREATIVE, LEVEL_STORAGE_VERSION_DEFAULT));
	vec.push_back(LevelSummary("Level3", "Level-3", 34567, 3456789, GAME_TYPE_CREATIVE, LEVEL_STORAGE_VERSION_DEFAULT));
	vec.push_back(LevelSummary("Level4", "Level-4", 45678, 4567890, GAME_TYPE_CREATIVE, LEVEL_STORAGE_VERSION_DEFAULT));
	vec.push_back(LevelSummary("Level5", "Level-5", 56789, 5678901, GAME_TYPE_CREATIVE, LEVEL_STORAGE_VERSION_DEFAULT));
#endif
}
