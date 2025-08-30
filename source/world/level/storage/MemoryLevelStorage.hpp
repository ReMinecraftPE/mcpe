/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LevelStorage.hpp"

class MemoryLevelStorage : public LevelStorage
{
public:
	LevelData* prepareLevel(Level*) override;
	ChunkStorage* createChunkStorage(Dimension*) override;
	void saveLevelData(LevelData* levelData, const std::vector<Player*>* players) override;
	void saveLevelData(LevelData* levelData) override;
	void closeAll() override;
};

