/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "ChunkSource.hpp"
#include "ChunkStorage.hpp"
class Level;

class ChunkCache : public ChunkSource
{
public:
	ChunkCache(Level*, ChunkStorage*, ChunkSource*);
	virtual ~ChunkCache();

	LevelChunk* create(int x, int z) override;
	LevelChunk* getChunk(int x, int z) override;
	bool hasChunk(int x, int z) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, int, int) override;
	bool shouldSave() override;
	void saveAll() override;
	int tick() override;
#ifdef ENH_IMPROVED_SAVING
	void saveUnsaved() override;
#endif

	LevelChunk* load(int, int);
	void save(LevelChunk*);

public:
	bool field_4 = true;
	LevelChunk* m_pEmptyChunk = nullptr;
	ChunkSource* m_pChunkSource = nullptr;
	ChunkStorage* m_pChunkStorage = nullptr;
	LevelChunk* m_chunkMap[C_MAX_CHUNKS_Z][C_MAX_CHUNKS_X];
	Level* m_pLevel = nullptr;
	LevelChunk* m_pLastChunk = nullptr;
	int m_LastChunkX = -999999999;
	int m_LastChunkZ = -999999999;
};

