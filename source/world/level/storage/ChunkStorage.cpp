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

LevelChunk* ChunkStorage::load(Level* a, int b, int c)
{
	return 0;
}

void ChunkStorage::save(Level* a, LevelChunk* b)
{
}

void ChunkStorage::saveEntities(Level* a, LevelChunk* b)
{
}

void ChunkStorage::saveAll(Level* a, std::vector<LevelChunk*>& b)
{
	for (auto chk : b)
	{
		save(a, chk);
	}
}

void ChunkStorage::tick()
{

}

void ChunkStorage::flush()
{

}
