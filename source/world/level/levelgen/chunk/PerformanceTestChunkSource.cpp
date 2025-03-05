/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PerformanceTestChunkSource.hpp"
#include "world/level/Level.hpp"

PerformanceTestChunkSource::PerformanceTestChunkSource()
{
	m_pLevel = nullptr;
}

LevelChunk* PerformanceTestChunkSource::create(const ChunkPos& pos)
{
	TileID* pData = new TileID[0x8000u];
	memset(pData, 0, 0x8000u * sizeof(TileID));

	for (int i = 0; i != 65; i++)
	{
		if (i > 59)
		{
			// alternating columns of dirt
			for (int j = 0; j < 16; j += 2)
			{
				for (int k = 0; k < 16; k += 2)
				{
					pData[i | (j << 11) | (k << 7)] = 3; // dirt
				}
			}
		}
		else
		{
			// alternating checkerboard patterns of dirt
			for (int j = (~i) & 1; j < 16; j += 2)
			{
				for (int k = i & 1; k < 16; k += 2)
				{
					pData[i | (j << 11) | (k << 7)] = 3; // dirt
				}
			}
		}
	}

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pData, pos);
	pChunk->recalcHeightmap();

	return pChunk;
}

std::string PerformanceTestChunkSource::gatherStats()
{
	return "PerformanceTestChunkSource";
}

// this sucks because it creates a new chunk every call to this function...
LevelChunk* PerformanceTestChunkSource::getChunk(const ChunkPos& pos)
{
	return create(pos);
}

LevelChunk* PerformanceTestChunkSource::getChunkDontCreate(const ChunkPos& pos)
{
	TileID* pData = new TileID[0x8000u];
	memset(pData, 0, 0x8000u * sizeof(TileID));

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pData, pos);
	return pChunk;
}

bool PerformanceTestChunkSource::hasChunk(const ChunkPos& pos)
{
	return true;
}

void PerformanceTestChunkSource::postProcess(ChunkSource* a, const ChunkPos& pos)
{

}

bool PerformanceTestChunkSource::shouldSave()
{
	return false;
}

int PerformanceTestChunkSource::tick()
{
	return 0;
}

