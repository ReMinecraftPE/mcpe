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

class Level;

class ChunkCache : public ChunkSource
{
public:
	ChunkCache(Level*, ChunkStorage*, ChunkSource*);
	virtual ~ChunkCache();

	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	bool hasChunk(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;
	bool shouldSave() override;
	void saveAll() override;
	int tick() override;
#ifdef ENH_IMPROVED_SAVING
	void saveUnsaved() override;
#endif

	LevelChunk* load(const ChunkPos& pos);
	void save(LevelChunk*);

public:
	bool field_4;
	LevelChunk* m_pEmptyChunk;
	ChunkSource* m_pChunkSource;
	ChunkStorage* m_pChunkStorage;
	LevelChunk* m_chunkMap[C_MAX_CHUNKS_Z][C_MAX_CHUNKS_X];
	Level* m_pLevel;
	LevelChunk* m_pLastChunk;
	ChunkPos m_lastChunkPos;
};

