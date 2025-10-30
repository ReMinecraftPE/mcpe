/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <string>
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "GameMods.hpp"
class Level;
class LevelChunk;

class ChunkSource
{
public:
	virtual ~ChunkSource();
	virtual bool hasChunk(const ChunkPos& pos) = 0;
	virtual LevelChunk* getChunk(const ChunkPos& pos) = 0;
	virtual LevelChunk* create(const ChunkPos& pos) = 0;
	virtual LevelChunk* getChunkDontCreate(const ChunkPos& pos) = 0;
	virtual void postProcess(ChunkSource*, const ChunkPos& pos) = 0;
	virtual int  tick() = 0;
	virtual bool shouldSave() = 0;
	virtual void saveAll();
	virtual std::string gatherStats() = 0;
#ifdef ENH_IMPROVED_SAVING
	virtual void saveUnsaved();
#endif
};

