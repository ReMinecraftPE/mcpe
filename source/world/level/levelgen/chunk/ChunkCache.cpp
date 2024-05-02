/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelChunk.hpp"
#include "ChunkCache.hpp"
#include "world/level/Level.hpp"

ChunkCache::ChunkCache(Level* pLevel, ChunkStorage* pStor, ChunkSource* pSrc)
{
	field_4 = true;
	m_pLevel = nullptr;
	m_pLastChunk = nullptr;
	m_lastChunkX = -999999999;
	m_lastChunkZ = -999999999;
	m_chunkOverX = 0;
	m_chunkOverZ = 0;

	m_pChunkSource = pSrc;
	m_pChunkStorage = pStor;
	m_pLevel = pLevel;

	m_pEmptyChunk = new EmptyLevelChunk(pLevel, nullptr, 0, 0);
	m_pEmptyChunk->field_236 = true;

	memset(m_chunkMap, 0, sizeof(m_chunkMap));
}

LevelChunk* ChunkCache::create(int x, int z)
{
	return getChunk(x, z);
}

LevelChunk* ChunkCache::getChunk(int x, int z)
{
	// get the last chunk quickly if needed
	if (m_lastChunkX == x && m_lastChunkZ == z && m_pLastChunk)
		return m_pLastChunk;

	if (!m_pLevel->m_bCalculatingInitialSpawn && !mayHaveChunk(x, z))
		return m_pEmptyChunk;

	int ix = x & (C_LOADED_CHUNKS_MAX - 1);
	int iz = z & (C_LOADED_CHUNKS_MAX - 1);

	if (!hasChunk(x, z))
	{
		LevelChunk* pOldChunk = m_chunkMap[iz][ix];
		if (pOldChunk)
		{
			m_chunkMap[iz][ix] = nullptr;
			pOldChunk->unload();
			save(pOldChunk);
			if (m_pChunkStorage)
				m_pChunkStorage->saveEntities(m_pLevel, pOldChunk);

			if (pOldChunk != m_pEmptyChunk) {
				pOldChunk->deleteBlockData();
				delete pOldChunk;
			}
		}

		LevelChunk* pChunk = load(x, z);
		if (pChunk)
		{
			m_chunkMap[iz][ix] = pChunk;
			pChunk->lightLava();

			int globalX = x * 16, globalZ = z * 16;
			for (int i = globalX, m = 0; m < 16; i++, m++)
			{
				for (int j = globalZ, n = 0; n < 16; j++, n++)
				{
					int height = m_pLevel->getHeightmap(i, j);
					if (height >= 0)
					{
						for (int k = height; k > 0; k--)
						{
							m_pLevel->updateLight(LightLayer::Sky,   globalX,   k, globalZ,   globalX,   k, globalZ);
							m_pLevel->updateLight(LightLayer::Block, globalX-1, k, globalZ-1, globalX+1, k, globalZ+1);
						}
					}
				}
			}
		}
		else
		{
			pChunk = m_pEmptyChunk;
			if (m_pChunkSource) {
				pChunk = m_pChunkSource->getChunk(x, z);
				pChunk->lightLava();
			}

			m_chunkMap[iz][ix] = pChunk;
		}

		pChunk = m_chunkMap[iz][ix];
		if (pChunk)
			pChunk->load();

		if (hasChunk(x, z) && !getChunk(x, z)->field_234 && hasChunk(x + 1, z + 1) && hasChunk(x, z + 1) && hasChunk(x + 1, z))
			postProcess(this, x, z);

		//@OVERSIGHT: redundant call twice to hasChunk(x-1, z), hasChunk(x,z-1), and hasChunk(x-1,z-1)
		if (hasChunk(x - 1, z) && !getChunk(x - 1, z)->field_234 && hasChunk(x - 1, z + 1) && hasChunk(x, z + 1) && hasChunk(x - 1, z))
			postProcess(this, x - 1, z);

		if (hasChunk(x, z - 1) && !getChunk(x, z - 1)->field_234 && hasChunk(x + 1, z - 1) && hasChunk(x + 1, z) && hasChunk(x, z - 1))
			postProcess(this, x, z - 1);

		if (hasChunk(x - 1, z - 1) && !getChunk(x - 1, z - 1)->field_234 && hasChunk(x - 1, z - 1) && hasChunk(x, z - 1) && hasChunk(x - 1, z))
			postProcess(this, x - 1, z - 1);
	}

	m_lastChunkX = x;
	m_lastChunkZ = z;
	m_pLastChunk = m_chunkMap[iz][ix];
	return m_chunkMap[iz][ix];
}

LevelChunk* ChunkCache::getChunkDontCreate(int x, int z)
{
	// get the last chunk quickly if needed
	if (m_lastChunkX == x && m_lastChunkZ == z && m_pLastChunk)
		return m_pLastChunk;

	if (!m_pLevel->m_bCalculatingInitialSpawn && !mayHaveChunk(x, z))
		return m_pEmptyChunk;

	int ix = x & (C_LOADED_CHUNKS_MAX - 1);
	int iz = z & (C_LOADED_CHUNKS_MAX - 1);

	if (!hasChunk(x, z))
	{
		LevelChunk* pOldChunk = m_chunkMap[iz][ix];
		if (pOldChunk)
		{
			pOldChunk->unload();
			save(pOldChunk);
			if (m_pChunkStorage)
				m_pChunkStorage->saveEntities(m_pLevel, pOldChunk);
		}

		// create an empty chunk
		LevelChunk* pChunk = m_pEmptyChunk;
		if (m_pChunkSource)
			pChunk = m_pChunkSource->getChunkDontCreate(x, z);

		m_chunkMap[iz][ix] = pChunk;
	}

	m_lastChunkX = x;
	m_lastChunkZ = z;
	m_pLastChunk = m_chunkMap[iz][ix];
	return m_chunkMap[iz][ix];
}

bool ChunkCache::mayHaveChunk(int x, int z)
{
	const int radius = C_LOADED_CHUNKS_MAX / 2;

	return
		x >= m_chunkOverX - radius &&
		z >= m_chunkOverZ - radius &&
		x <= m_chunkOverX + radius &&
		z <= m_chunkOverZ + radius;
}

bool ChunkCache::hasChunk(int x, int z)
{
	if (!mayHaveChunk(x, z))
		return false;

	if (m_lastChunkX == x && m_lastChunkZ == z && m_pLastChunk)
		return true;

	int ix = x & (C_LOADED_CHUNKS_MAX - 1);
	int iz = z & (C_LOADED_CHUNKS_MAX - 1);
	if (!m_chunkMap[iz][ix])
		return false;

	if (m_chunkMap[iz][ix] == m_pEmptyChunk)
		return true;

	return m_chunkMap[iz][ix]->isAt(x, z);
}

int ChunkCache::tick()
{
	if (m_pChunkStorage)
		m_pChunkStorage->tick();

	return m_pChunkSource->tick();
}

void ChunkCache::postProcess(ChunkSource* pChkSrc, int x, int z)
{
	LevelChunk* pChunk = getChunk(x, z);
	if (!pChunk->field_234)
	{
		pChunk->field_234 = 1;
		if (m_pChunkSource)
		{
			m_pChunkSource->postProcess(m_pChunkSource, x, z);
			pChunk->clearUpdateMap();
		}
	}
}

void ChunkCache::save(LevelChunk* pChunk)
{
	return; // TODO
	if (m_pChunkStorage)
	{
		pChunk->field_23C = m_pLevel->getTime();
		m_pChunkStorage->save(m_pLevel, pChunk);
	}
}

void ChunkCache::saveAll()
{
	if (!m_pChunkStorage) return;

	std::vector<LevelChunk*> chunksToSave;

	for (int i = 0; i < C_MAX_CHUNKS_Z; i++)
	{
		for (int j = 0; j < C_MAX_CHUNKS_X; j++)
		{
			chunksToSave.push_back(m_pLevel->getChunk(j, i));
		}
	}

	m_pChunkStorage->saveAll(m_pLevel, chunksToSave);
}

#ifdef ENH_IMPROVED_SAVING

void ChunkCache::saveUnsaved()
{
	if (!m_pChunkStorage) return;
	return; // TODO

	std::vector<LevelChunk*> chunksToSave;

	for (int i = 0; i < C_MAX_CHUNKS_Z; i++)
	{
		for (int j = 0; j < C_MAX_CHUNKS_X; j++)
		{
			LevelChunk* pChunk = m_pLevel->getChunk(j, i);
			if (!pChunk->m_bUnsaved)
				continue;

			chunksToSave.push_back(pChunk);
		}
	}

	m_pChunkStorage->saveAll(m_pLevel, chunksToSave);
}

#endif

void ChunkCache::setChunkOver(int x, int z)
{
	m_chunkOverX = x;
	m_chunkOverZ = z;
}

bool ChunkCache::shouldSave()
{
	return true;
}

std::string ChunkCache::gatherStats()
{
	return "ChunkCache: 1024";
}

ChunkCache::~ChunkCache()
{
	for (int i = 0; i < C_LOADED_CHUNKS_MAX; i++)
	{
		for (int j = 0; j < C_LOADED_CHUNKS_MAX; j++)
		{
			LevelChunk* pChk = m_chunkMap[i][j];
			assert(pChk != m_pEmptyChunk);

			if (pChk)
			{
				pChk->deleteBlockData();
				SAFE_DELETE(pChk);
			}
		}
	}

	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pEmptyChunk);
}

LevelChunk* ChunkCache::load(int x, int z)
{
	if (!m_pChunkStorage)
		return m_pEmptyChunk;

	return nullptr; // TODO

	LevelChunk* pChk = m_pChunkStorage->load(m_pLevel, x, z);
	if (pChk)
		pChk->field_23C = m_pLevel->getTime();
	
	return pChk;
}
