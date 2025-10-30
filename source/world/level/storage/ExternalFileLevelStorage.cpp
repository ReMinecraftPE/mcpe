/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <stdint.h>

#include "ExternalFileLevelStorage.hpp"

#include "common/Logger.hpp"
#include "nbt/CompoundTag.hpp"
#include "nbt/NbtIo.hpp"
#include "network/RakIO.hpp"
#include "world/entity/EntityFactory.hpp"
#include "world/level/Level.hpp"
#include "thirdparty/raknet/GetTime.h"

#ifndef DEMO

#define C_CHUNKS_TO_SAVE_PER_TICK (2)

ExternalFileLevelStorage::ExternalFileLevelStorage(const std::string& name, const std::string& path, bool forceConversion) :
	m_levelName(name),
	m_levelDirPath(path),
	m_timer(0),
	m_storageVersion(LEVEL_STORAGE_VERSION_DEFAULT),
	m_lastEntitySave(-999999),
    m_bForceConversion(forceConversion)
{
	m_pRegionFile = nullptr;
	m_pLevel = nullptr;

	createFolderIfNotExists(m_levelDirPath.c_str());

	std::string datLevel  = m_levelDirPath + "/" + "level.dat";
	std::string datPlayer = m_levelDirPath + "/" + "player.dat";

	m_pLevelData = new LevelData;
	if (!readLevelData(datLevel, *m_pLevelData))
	{
		delete m_pLevelData;
		m_pLevelData = nullptr;
		return;
	}

	m_storageVersion = m_pLevelData->getStorageVersion();

    if (m_storageVersion <= 1)
    {
        readPlayerData(datPlayer, *m_pLevelData);
    }
}

ExternalFileLevelStorage::~ExternalFileLevelStorage()
{
	SAFE_DELETE(m_pRegionFile);
	SAFE_DELETE(m_pLevelData);
}

void ExternalFileLevelStorage::_setLevelData(LevelData* levelData)
{
	if (m_pLevelData == levelData)
		return;

	SAFE_DELETE(m_pLevelData);

	m_pLevelData = levelData;
}

LevelData* ExternalFileLevelStorage::prepareLevel(Level* level)
{
	m_pLevel = level;
	return m_pLevelData;
}

ChunkStorage* ExternalFileLevelStorage::createChunkStorage(Dimension* pDim)
{
	return this;
}

void ExternalFileLevelStorage::saveLevelData(const std::string& levelPath, LevelData* levelData, const std::vector<Player*>* players)
{
	std::string pathBase = levelPath + "/";
	std::string pathNew = pathBase + "level.dat_new";
	std::string path    = pathBase + "level.dat";
	std::string pathOld = pathBase + "level.dat_old";

#ifndef ENH_DISABLE_FORCED_SAVE_UPGRADES
    if (m_bForceConversion)
    {
        // Forces world to upgrade to new default storage version.
        levelData->setStorageVersion(LEVEL_STORAGE_VERSION_DEFAULT);
    }
#endif

	writeLevelData(path, *levelData, players);

	if (levelData->getStorageVersion() == 1)
	{
		if (players != nullptr)
			savePlayerData(*levelData, *players);
	}

	_setLevelData(levelData);
}

void ExternalFileLevelStorage::saveLevelData(LevelData* levelData, const std::vector<Player*>* players)
{
	saveLevelData(m_levelDirPath, levelData, players);
}

void ExternalFileLevelStorage::savePlayerData(LevelData& levelData, const std::vector<Player*>& players)
{
	if (levelData.getStorageVersion() > 1)
		return; // we shouldn't be saving player.dat files for V2 and greater

	if (players.empty())
		return;

	FILE* pFile = fopen((m_levelDirPath + "/" + "player.dat").c_str(), "wb");
	if (!pFile)
	{
		LOG_W("Not saving player data");
		return;
	}

	levelData.m_LocalPlayerData.savePlayer(*players[0]);

	int nPlayers = 1;
	fwrite(&nPlayers, sizeof nPlayers, 1, pFile);

	int nSizePD = 80;
	fwrite(&nSizePD,  sizeof nSizePD, 1, pFile);

	// @NOTE: No reason to swap elementCount and elementSize here. I understood it the
	// last time - to check whether the data loaded all the way. However, no checks are
	// done here.
	fwrite(&levelData.m_LocalPlayerData, 1, nSizePD, pFile);

	fclose(pFile);
}

void ExternalFileLevelStorage::saveGame(Level* level)
{
	// I don't know why it makes me specify this manually
	ChunkStorage::saveEntities(level);
}

void ExternalFileLevelStorage::closeAll()
{
}

void ExternalFileLevelStorage::tick()
{
	m_timer++;
	if (m_timer % 50 != 0 || !m_pLevel)
		return;

	ChunkPos cp(0, 0);
	for (cp.z = 0; cp.z < C_MAX_CHUNKS_Z; cp.z++)
	{
		for (cp.x = 0; cp.x < C_MAX_CHUNKS_X; cp.x++)
		{
			LevelChunk* pChunk = m_pLevel->getChunk(cp);
			if (!pChunk || !pChunk->m_bUnsaved)
				continue;

			int index = cp.x + cp.z * 16;

			std::list<UnsavedLevelChunk>::iterator iter = m_unsavedLevelChunks.begin();
			for (; iter != m_unsavedLevelChunks.end(); ++iter)
			{
				if (iter->m_index == index)
				{
					iter->m_foundTime = RakNet::GetTimeMS();
					break;
				}
			}

			if (iter == m_unsavedLevelChunks.end())
			{
				UnsavedLevelChunk ulc = { index, int(RakNet::GetTimeMS()), pChunk };
				m_unsavedLevelChunks.push_back(ulc);
			}

			pChunk->m_bUnsaved = false;
		}
	}

	int count = 0;
	while (count < C_CHUNKS_TO_SAVE_PER_TICK && !m_unsavedLevelChunks.empty())
	{
		count++;

		std::list<UnsavedLevelChunk>::iterator iter = m_unsavedLevelChunks.begin();
		for (std::list<UnsavedLevelChunk>::iterator it2 = m_unsavedLevelChunks.begin(); it2 != m_unsavedLevelChunks.end(); ++it2)
		{
			if (iter->m_foundTime > it2->m_foundTime)
				iter = it2;
		}

		LevelChunk* pChunk = iter->m_pChunk;
		m_unsavedLevelChunks.erase(iter);

		save(m_pLevel, pChunk);
	}

	if (m_timer - m_lastEntitySave > 1200)
		saveEntities(m_pLevel, nullptr);
}

void ExternalFileLevelStorage::flush()
{
}

LevelChunk* ExternalFileLevelStorage::load(Level* level, const ChunkPos& pos)
{
	if (!m_pRegionFile)
	{
		m_pRegionFile = new RegionFile(m_levelDirPath);

		if (!m_pRegionFile->open())
		{
			SAFE_DELETE(m_pRegionFile);
			m_pRegionFile = nullptr;

			return nullptr;
		}
	}

	RakNet::BitStream* pBitStream = nullptr;
	if (!m_pRegionFile->readChunk(pos, &pBitStream))
		return nullptr;

	pBitStream->ResetReadPointer();

	TileID* pData = new TileID[16 * 16 * 128];
	pBitStream->Read((char*)pData, 16 * 16 * 128 * sizeof(TileID));

	LevelChunk* pChunk = new LevelChunk(level, pData, pos);
	pBitStream->Read((char*)pChunk->m_tileData.m_data, 16 * 16 * 128 / 2);

	if (m_storageVersion >= 1)
	{
		pBitStream->Read((char*)pChunk->m_lightSky.m_data, 16 * 16 * 128 / 2);
		pBitStream->Read((char*)pChunk->m_lightBlk.m_data, 16 * 16 * 128 / 2);
	}

	pBitStream->Read((char*)pChunk->m_updateMap, sizeof pChunk->m_updateMap);
	
	delete pBitStream->GetData();
	delete pBitStream;

	pChunk->recalcHeightmap();
	pChunk->m_bUnsaved = false;
	pChunk->field_234 = true;
	pChunk->field_237 = true;

	return pChunk;
}

void ExternalFileLevelStorage::loadEntities(Level* level, LevelChunk* chunk)
{
	m_lastEntitySave = m_timer;

	FILE* pFile = fopen((m_levelDirPath + "/entities.dat").c_str(), "rb");
	if (!pFile)
		return;

	char formatId[4];
	fread(formatId, 1, 4, pFile);
	int formatVersion;
	fread(&formatVersion, 4, 1, pFile);
	unsigned int size;
	fread(&size, 4, 1, pFile);
	
	long v6 = ftell(pFile);
	fseek(pFile, 0, 2);
	long v7 = ftell(pFile);
	fseek(pFile, v6, 0);

	if (size <= v7 - v6 && size > 0)
	{
		uint8_t* data = new uint8_t[size];
		fread(data, 1, size, pFile);

		RakNet::BitStream bs(data, size, false);
		RakDataInput dis = RakDataInput(bs);

		CompoundTag* tag = NbtIo::read(dis);
		if (tag)
		{
			if (tag->getId() == Tag::TAG_TYPE_COMPOUND)
			{
				const ListTag* entitiesTag = tag->getList("Entities");
				if (entitiesTag)
				{
					const std::vector<Tag*>& entities = entitiesTag->rawView();
					for (std::vector<Tag*>::const_iterator it = entities.begin(); it != entities.end(); it++)
					{
						const Tag* betterTag = *it;
						if (!betterTag || betterTag->getId() != Tag::TAG_TYPE_COMPOUND)
							continue;

						Entity* entity = EntityFactory::LoadEntity(*(CompoundTag*)betterTag, level);
						if (entity)
							level->addEntity(entity);
					}
				}
			}
            
            tag->deleteChildren();
            delete tag;
		}

		if (data)
			delete[] data;
	}

	fclose(pFile);
}

void ExternalFileLevelStorage::save(Level* level, LevelChunk* chunk)
{
	if (!m_pRegionFile)
		m_pRegionFile = new RegionFile(m_levelDirPath);

	if (!m_pRegionFile->open())
	{
		SAFE_DELETE(m_pRegionFile);
		m_pRegionFile = nullptr;

		LOG_W("Not saving :(   (x: %d  z: %d)", chunk->m_chunkPos.x, chunk->m_chunkPos.z);
		return;
	}

	RakNet::BitStream bs;
	bs.Write((const char*)chunk->m_pBlockData,        16 * 16 * 128 * sizeof(TileID));
	bs.Write((const char*)chunk->m_tileData.m_data, chunk->m_tileData.m_size);

	if (m_pLevelData->getStorageVersion() >= 1)
	{
		bs.Write((const char*)chunk->m_lightSky.m_data, chunk->m_lightSky.m_size);
		bs.Write((const char*)chunk->m_lightBlk.m_data, chunk->m_lightBlk.m_size);
	}

	bs.Write((const char*)chunk->m_updateMap, sizeof chunk->m_updateMap);

	m_pRegionFile->writeChunk(chunk->m_chunkPos, bs);
}

void ExternalFileLevelStorage::saveEntities(Level* level, LevelChunk* chunk)
{
	m_lastEntitySave = m_timer;
	//getTimeS();
	ListTag* entitiesTag = new ListTag();

	const EntityVector* entities = level->getAllEntities();
	for (EntityVector::const_iterator it = entities->begin(); it != entities->end(); it++)
	{
		const Entity* entity = *it;
		CompoundTag* tag = new CompoundTag();

		if (!entity->save(*tag))
			continue;

		entitiesTag->add(tag);
	}

	CompoundTag tag = CompoundTag();
	tag.put("Entities", entitiesTag);
	RakNet::BitStream bs;
	RakDataOutput dos = RakDataOutput(bs);
	NbtIo::write(tag, dos);
    tag.deleteChildren();

	unsigned int size = bs.GetNumberOfBytesUsed();

	FILE* pFile = fopen((m_levelDirPath + "/entities.dat").c_str(), "wb");
	if (pFile)
	{
		int formatVersion = 1; // I'm assuming it's a version number
		fwrite("ENT", 1, 4, pFile);
		fwrite(&formatVersion, 4, 1, pFile);
		fwrite(&size, 4, 1, pFile);
		fwrite(bs.GetData(), 1, size, pFile);
		fclose(pFile);
	}

	//getTimeS();
}

bool ExternalFileLevelStorage::readLevelData(const std::string& path, LevelData& levelData)
{
	FILE* pFile = fopen(path.c_str(), "rb");
	if (!pFile)
		return false;

	int version = 0, length = 0;
	if (fread(&version, sizeof(int), 1, pFile) != 1)
	{
	_cleanup:
		fclose(pFile);
		return false;
	}

	if (fread(&length, sizeof(int), 1, pFile) != 1)
		goto _cleanup;

	uint8_t* data = new uint8_t[length];

	if (fread(data, sizeof(uint8_t), length, pFile) != length)
	{
		SAFE_DELETE_ARRAY(data);
		goto _cleanup;
	}

	RakNet::BitStream bs(data, length, false);
	if (version == 1)
	{
		levelData.v1_read(bs, version);
	}
	else if (version == 2)
	{
		levelData.read(bs, version);
	}

	SAFE_DELETE_ARRAY(data);
	fclose(pFile);

	return true;
}

bool ExternalFileLevelStorage::readPlayerData(const std::string& path, LevelData& levelData)
{
	FILE* pFile = fopen(path.c_str(), "rb");
	if (!pFile)
		return false;

	// don't know if it's actually nPlayers or version
	int nPlayers = 0, size = 0;
	if (fread(&nPlayers, sizeof(int), 1, pFile) != 1)
		goto _cleanup;

	if (fread(&size, sizeof(int), 1, pFile) != 1)
		goto _cleanup;

	if (nPlayers != 1)
		goto _cleanup;

	if (fread(&levelData.m_LocalPlayerData, 1, sizeof levelData.m_LocalPlayerData, pFile) == size)
		levelData.m_nPlayers = nPlayers;

	fclose(pFile);
	return true;

_cleanup:
	fclose(pFile);
	return false;
}

bool ExternalFileLevelStorage::writeLevelData(const std::string& path, const LevelData& levelData, const std::vector<Player*>* players)
{
	FILE* pFile = fopen(path.c_str(), "wb");
	if (!pFile)
		return false;

	RakNet::BitStream bs;
	if (levelData.getStorageVersion() == 1)
	{
		levelData.v1_write(bs);
	}
	else
	{
		levelData.write(bs, players);
	}

	int storageVersion = levelData.getStorageVersion();
	fwrite(&storageVersion, sizeof(int), 1, pFile);

	int length = bs.GetNumberOfBytesUsed();
	fwrite(&length, sizeof(int), 1, pFile);
	fwrite(bs.GetData(), 1, length, pFile);
	fclose(pFile);

	return true;
}

#endif
