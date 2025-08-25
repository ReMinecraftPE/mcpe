/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MemoryLevelStorage.hpp"
#include "MemoryChunkStorage.hpp"
#include "LevelData.hpp"

LevelData* MemoryLevelStorage::prepareLevel(Level* pLevel)
{
	return nullptr;
}

ChunkStorage* MemoryLevelStorage::createChunkStorage(Dimension* pDim)
{
	return new MemoryChunkStorage;
}

void MemoryLevelStorage::saveLevelData(LevelData* levelData, const std::vector<Player*>* players)
{
}

void MemoryLevelStorage::saveLevelData(LevelData* levelData)
{
}

void MemoryLevelStorage::closeAll()
{
}
