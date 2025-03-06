/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include "world/level/levelgen/chunk/ChunkPos.hpp"

class Level;
class LevelChunk;

class ChunkStorage
{
public:
	virtual ~ChunkStorage();
	virtual LevelChunk* load(Level*, const ChunkPos& pos);
	virtual void save(Level*, LevelChunk*);
	virtual void saveEntities(Level*, LevelChunk*);
	virtual void saveAll(Level*, std::vector<LevelChunk*>&);
	virtual void tick();
	virtual void flush();
};

