/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "ChunkSource.hpp"
#include "world/level/storage/ChunkStorage.hpp"
#include "world/level/Level.hpp"

// Note, MUST be a power of 2!
#define C_LOADED_CHUNKS_MAX (32)

class Level;

class ChunkCache : public ChunkSource
{
public:
	ChunkCache(Level*, ChunkStorage*, ChunkSource*);
	virtual ~ChunkCache();

	LevelChunk* create(int x, int z) override;
	LevelChunk* getChunk(int x, int z) override;
	LevelChunk* getChunkDontCreate(int x, int z) override;
	bool hasChunk(int x, int z) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, int, int) override;
	bool shouldSave() override;
	void saveAll() override;
	int tick() override;
	void setChunkOver(int x, int z) override;
#ifdef ENH_IMPROVED_SAVING
	void saveUnsaved() override;
#endif

	LevelChunk* load(int, int);
	void save(LevelChunk*);
	bool mayHaveChunk(int x, int z) override;

public:
	bool field_4;
	LevelChunk* m_pEmptyChunk;
	ChunkSource* m_pChunkSource;
	ChunkStorage* m_pChunkStorage;
	LevelChunk* m_chunkMap[C_LOADED_CHUNKS_MAX][C_LOADED_CHUNKS_MAX];
	Level* m_pLevel;
	LevelChunk* m_pLastChunk;
	int m_lastChunkX;
	int m_lastChunkZ;
	int m_chunkOverX;
	int m_chunkOverZ;
};

