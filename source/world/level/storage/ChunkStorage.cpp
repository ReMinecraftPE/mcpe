/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChunkStorage.hpp"

ChunkStorage::~ChunkStorage()
{
}

LevelChunk* ChunkStorage::load(Level* level, const ChunkPos& pos)
{
	return 0;
}

void ChunkStorage::save(Level* level, LevelChunk* chunk)
{
}

void ChunkStorage::saveEntities(Level* level, LevelChunk* chunk)
{
}

void ChunkStorage::saveAll(Level* a, std::vector<LevelChunk*>& b)
{
	for (std::vector<LevelChunk*>::iterator it = b.begin(); it != b.end(); it++)
	{
		save(a, *it);
	}
}

void ChunkStorage::tick()
{

}

void ChunkStorage::flush()
{

}
