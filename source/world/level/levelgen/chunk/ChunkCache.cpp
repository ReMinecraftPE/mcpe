/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelChunk.hpp"
#include "ChunkCache.hpp"
#include "world/level/Level.hpp"
#include "EmptyLevelChunk.hpp"

ChunkCache::ChunkCache(Level* pLevel, ChunkStorage* pStor, ChunkSource* pSrc)
{
	field_4 = true;
	m_pLevel = nullptr;
	m_pLastChunk = nullptr;
	m_lastChunkPos = ChunkPos(-999999999, -999999999);

	m_pChunkSource = pSrc;
	m_pChunkStorage = pStor;
	m_pLevel = pLevel;

	m_pEmptyChunk = new EmptyLevelChunk(pLevel, nullptr, ChunkPos(0, 0));
	m_pEmptyChunk->field_236 = true;

	memset(m_chunkMap, 0, sizeof(m_chunkMap));
}

LevelChunk* ChunkCache::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

LevelChunk* ChunkCache::getChunk(const ChunkPos& pos)
{
	// get the last chunk quickly if needed
	if (m_lastChunkPos == pos)
	{
		if (m_pLastChunk)
			return m_pLastChunk;
	}

	if (pos.x < 0 || pos.z < 0 || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return m_pEmptyChunk;

	if (!hasChunk(pos))
	{
		LevelChunk* pOldChunk = m_chunkMap[pos.z][pos.x];
		if (pOldChunk)
		{
			pOldChunk->unload();
			save(pOldChunk);
			if (m_pChunkStorage)
				m_pChunkStorage->saveEntities(m_pLevel, pOldChunk);
		}

		LevelChunk* pChunk = load(pos);
		if (pChunk)
		{
			m_chunkMap[pos.z][pos.x] = pChunk;
			pChunk->lightLava();

			TilePos global(pos, 0);
			for (int i = global.x, m = 0; m < 16; i++, m++)
			{
				for (int j = global.z, n = 0; n < 16; j++, n++)
				{
					int height = m_pLevel->getHeightmap(TilePos(i, 0, j));
					if (height >= 0)
					{
						for (int k = height; k > 0; k--)
						{
							m_pLevel->updateLight(LightLayer::Sky,   TilePos(global.x,   k, global.z),   TilePos(global.x,   k, global.z));
							m_pLevel->updateLight(LightLayer::Block, TilePos(global.x-1, k, global.z-1), TilePos(global.x+1, k, global.z+1));
						}
					}
				}
			}
		}
		else
		{
			pChunk = m_pEmptyChunk;
			if (m_pChunkSource)
				pChunk = m_pChunkSource->getChunk(pos);

			m_chunkMap[pos.z][pos.x] = pChunk;
			pChunk->lightLava();
		}

		pChunk = m_chunkMap[pos.z][pos.x];
		if (pChunk)
			pChunk->load();

		if (!pChunk->field_234 && hasChunk(ChunkPos(pos.x + 1, pos.z + 1)) && hasChunk(ChunkPos(pos.x, pos.z + 1)) && hasChunk(ChunkPos(pos.x + 1, pos.z)))
			postProcess(this, pos);

		//@OVERSIGHT: redundant call twice to hasChunk(x-1, z), hasChunk(x,z-1), and hasChunk(x-1,z-1)
		if (hasChunk(ChunkPos(pos.x - 1, pos.z)) && !getChunk(ChunkPos(pos.x - 1, pos.z))->field_234 && hasChunk(ChunkPos(pos.x - 1, pos.z + 1)) && hasChunk(ChunkPos(pos.x, pos.z + 1)) && hasChunk(ChunkPos(pos.x - 1, pos.z)))
			postProcess(this, ChunkPos(pos.x - 1, pos.z));

		if (hasChunk(ChunkPos(pos.x, pos.z - 1)) && !getChunk(ChunkPos(pos.x, pos.z - 1))->field_234 && hasChunk(ChunkPos(pos.x + 1, pos.z - 1)) && hasChunk(ChunkPos(pos.x + 1, pos.z)) && hasChunk(ChunkPos(pos.x, pos.z - 1)))
			postProcess(this, ChunkPos(pos.x, pos.z - 1));

		if (hasChunk(pos - 1) && !getChunk(pos - 1)->field_234 && hasChunk(pos - 1) && hasChunk(ChunkPos(pos.x, pos.z - 1)) && hasChunk(ChunkPos(pos.x - 1, pos.z)))
			postProcess(this, pos - 1);
	}

	m_lastChunkPos = pos;
	m_pLastChunk = m_chunkMap[pos.z][pos.x];
	return m_chunkMap[pos.z][pos.x];
}

LevelChunk* ChunkCache::getChunkDontCreate(const ChunkPos& pos)
{
	// get the last chunk quickly if needed
	if (m_lastChunkPos == pos)
	{
		if (m_pLastChunk)
			return m_pLastChunk;
	}

	if (pos.x < 0 || pos.z < 0 || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return m_pEmptyChunk;

	if (!hasChunk(pos))
	{
		LevelChunk* pOldChunk = m_chunkMap[pos.z][pos.x];
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
			pChunk = m_pChunkSource->getChunkDontCreate(pos);

		m_chunkMap[pos.z][pos.x] = pChunk;
	}

	m_lastChunkPos = pos;
	m_pLastChunk = m_chunkMap[pos.z][pos.x];
	return m_pLastChunk;
}

bool ChunkCache::hasChunk(const ChunkPos& pos)
{
	if (pos.x < 0 || pos.z < 0)
		return true;

	if (pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return true;

	if (m_lastChunkPos == pos)
		return true;

	LevelChunk* pChunk = m_chunkMap[pos.z][pos.x];
	if (!pChunk)
		return false;

	if (pChunk == m_pEmptyChunk)
		return true;

	return pChunk->isAt(pos);
}

int ChunkCache::tick()
{
	if (m_pChunkStorage)
		m_pChunkStorage->tick();

	return m_pChunkSource->tick();
}

void ChunkCache::postProcess(ChunkSource* pChkSrc, const ChunkPos& pos)
{
	if (pos.x < 0 || pos.z < 0 || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return;

	LevelChunk* pChunk = getChunk(pos);
	if (!pChunk->field_234)
	{
		pChunk->field_234 = 1;
		if (m_pChunkSource)
		{
			m_pChunkSource->postProcess(m_pChunkSource, pos);
			pChunk->clearUpdateMap();
		}
	}
}

void ChunkCache::save(LevelChunk* pChunk)
{
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

	ChunkPos pos(0, 0);
	for (pos.z = 0; pos.z < C_MAX_CHUNKS_Z; pos.z++)
	{
		for (pos.x = 0; pos.x < C_MAX_CHUNKS_X; pos.x++)
		{
			chunksToSave.push_back(m_pLevel->getChunk(pos));
		}
	}

	m_pChunkStorage->saveAll(m_pLevel, chunksToSave);
}

#ifdef ENH_IMPROVED_SAVING

void ChunkCache::saveUnsaved()
{
	if (!m_pChunkStorage) return;

	std::vector<LevelChunk*> chunksToSave;

	ChunkPos pos(0, 0);
	for (pos.z = 0; pos.z < C_MAX_CHUNKS_Z; pos.z++)
	{
		for (pos.x = 0; pos.x < C_MAX_CHUNKS_X; pos.x++)
		{
			LevelChunk* pChunk = m_pLevel->getChunk(pos);
			if (!pChunk->m_bUnsaved)
				continue;

			chunksToSave.push_back(pChunk);
		}
	}

	m_pChunkStorage->saveAll(m_pLevel, chunksToSave);
}

#endif

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
	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pEmptyChunk);

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
		{
			LevelChunk* pChk = m_chunkMap[i][j];
			if (pChk)
			{
				pChk->deleteBlockData();
				SAFE_DELETE(pChk);
			}
		}
}

LevelChunk* ChunkCache::load(const ChunkPos& pos)
{
	if (!m_pChunkStorage)
		return m_pEmptyChunk;

	if (pos.x < 0 || pos.z < 0 || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return m_pEmptyChunk;

	LevelChunk* pChk = m_pChunkStorage->load(m_pLevel, pos);
	if (pChk)
		pChk->field_23C = m_pLevel->getTime();
	
	return pChk;
}
