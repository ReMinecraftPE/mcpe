/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "ChunkSource.hpp"

class Level;

class TestChunkSource : public ChunkSource
{
public:
	TestChunkSource(Level*);
	virtual ~TestChunkSource();

	LevelChunk* generateChunk(const ChunkPos& pos);
	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	bool hasChunk(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;
	bool shouldSave() override;
	int tick() override;

public:
	Level* m_pLevel;

	LevelChunk* m_chunkMap[C_MAX_CHUNKS_Z][C_MAX_CHUNKS_X];
	LevelChunk* m_pEmptyChunk;
	LevelChunk* m_pLastChunk;
	ChunkPos m_lastChunkPos;
};
