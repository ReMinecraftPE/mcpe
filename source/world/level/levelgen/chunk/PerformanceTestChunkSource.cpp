/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PerformanceTestChunkSource.hpp"
#include "world/level/Level.hpp"

LevelChunk* PerformanceTestChunkSource::create(int x, int z)
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

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pData, x, z);
	pChunk->recalcHeightmap();

	return pChunk;
}

std::string PerformanceTestChunkSource::gatherStats()
{
	return "PerformanceTestChunkSource";
}

// this sucks because it creates a new chunk every call to this function...
LevelChunk* PerformanceTestChunkSource::getChunk(int x, int z)
{
	return create(x, z);
}

bool PerformanceTestChunkSource::hasChunk(int x, int z)
{
	return true;
}

void PerformanceTestChunkSource::postProcess(ChunkSource* a, int b, int c)
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

