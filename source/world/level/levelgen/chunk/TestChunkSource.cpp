/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TestChunkSource.hpp"
#include "Level.hpp"

TestChunkSource::TestChunkSource(Level* pLevel)
{
	m_pLevel = pLevel;
	memset(m_chunkMap, 0, sizeof m_chunkMap);

	m_pEmptyChunk = new EmptyLevelChunk(pLevel, nullptr, 0, 0);

	m_lastChunkX = -99999;
	m_lastChunkZ = -99999;
	m_pLastChunk = m_pEmptyChunk;
}

TestChunkSource::~TestChunkSource()
{
	for (int z = 0; z < C_MAX_CHUNKS_Z; z++)
	{
		for (int x = 0; x < C_MAX_CHUNKS_X; x++)
		{
			if (m_chunkMap[z][x])
				delete m_chunkMap[z][x];
		}
	}
}

LevelChunk* TestChunkSource::generateChunk(int x, int z)
{
	//@NOTE: This gets called a dangerous amount of recursions (for the entire damn world!)
	// We really should fix this...

	if (x < 0 || z < 0 || x >= C_MAX_CHUNKS_X || z >= C_MAX_CHUNKS_Z)
		return nullptr;
	
	if (m_chunkMap[z][x])
		return m_chunkMap[z][x];

	TileID* pData = new TileID[0x8000u];
	memset(pData, 0, 0x8000u * sizeof(TileID));

	for (int i = 0; i != 66; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 16; k++)
			{
				TileID* p = &pData[i | (j << 11) | (k << 7)];

				if (i == 0)
					*p = TILE_BEDROCK;
				//else if (j == 0 || k == 0)
				//	*p = TILE_AIR;
				else if (i == 65) {
					/*if (rand() % 10 == 0)
						*p = TILE_ROSE;
					else if (rand() % 10 == 0)
						*p = TILE_FLOWER;
					else if (rand() % 10 == 0)
						*p = TILE_TOPSNOW;*/
				}
				else if (i > 64)
					*p = TILE_AIR;
				else if (i == 64) {
					//if ((j + k) % 2 == 0)
					*p = TILE_GRASS;
				}
				else if (i > 60)
					*p = TILE_DIRT;
				else if (i > 3)
					*p = TILE_STONE;
				else
					*p = TILE_STONEBRICK;

				// generate a hole at (128,0,128) for testing
				//if (i == 0 && j == 0 && k == 0 && x == 8 && z == 8)
				//	*p = TILE_WOOD;
			}
		}
	}

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pData, x, z);

	m_chunkMap[z][x] = pChunk;

	/*
	// block light updates during level generation
	bool bOldState = m_pLevel->m_bUpdateLights;
	m_pLevel->m_bUpdateLights = false;

	// encode this chunk's X/Z coordinate by setting bits in it
	pChunk->setTile(0, 65, 0, TILE_WOOD);
	pChunk->setTile(0, 65, 1, TILE_STONE);
	pChunk->setTile(3+1, 65, 0, (x & (1 << 0)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(2+1, 65, 0, (x & (1 << 1)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(1+1, 65, 0, (x & (1 << 2)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(0+1, 65, 0, (x & (1 << 3)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(3+1, 65, 1, (z & (1 << 0)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(2+1, 65, 1, (z & (1 << 1)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(1+1, 65, 1, (z & (1 << 2)) ? TILE_STONEBRICK : TILE_BEDROCK);
	pChunk->setTile(0+1, 65, 1, (z & (1 << 3)) ? TILE_STONEBRICK : TILE_BEDROCK);

	m_pLevel->m_bUpdateLights = bOldState;

	pChunk->setBrightness(LightLayer::Block, 0, 0, 0, 3);
	pChunk->setBrightness(LightLayer::Block, 0, 1, 0, 4);
	pChunk->setBrightness(LightLayer::Block, 0, 2, 0, 5);
	pChunk->setBrightness(LightLayer::Block, 0, 3, 0, 6);
	int br1 = pChunk->getBrightness(LightLayer::Block, 0, 0, 0);
	int br2 = pChunk->getBrightness(LightLayer::Block, 0, 1, 0);
	int br3 = pChunk->getBrightness(LightLayer::Block, 0, 2, 0);
	int br4 = pChunk->getBrightness(LightLayer::Block, 0, 3, 0);
	assert(br1  == 3);
	assert(br2  == 4);
	assert(br3  == 5);
	assert(br4  == 6);
	*/

	pChunk->recalcHeightmap();

	return pChunk;
}

LevelChunk* TestChunkSource::create(int x, int z)
{
	if (m_lastChunkX == x && m_lastChunkZ == z)
		return m_pLastChunk;

	LevelChunk* pChk = generateChunk(x, z);
	if (!pChk)
		pChk = m_pEmptyChunk;

	m_lastChunkX = x;
	m_lastChunkZ = z;
	m_pLastChunk = pChk;

	return pChk;
}

std::string TestChunkSource::gatherStats()
{
	return "TestChunkSource";
}

LevelChunk* TestChunkSource::getChunk(int x, int z)
{
	return create(x, z);
}

bool TestChunkSource::hasChunk(int x, int z)
{
	return true;
}

void TestChunkSource::postProcess(ChunkSource* a, int b, int c)
{

}

bool TestChunkSource::shouldSave()
{
	return false;
}

int TestChunkSource::tick()
{
	return 0;
}

