#include "ExternalFileLevelStorage.hpp"
#include "LevelChunk.hpp"

ExternalFileLevelStorage::ExternalFileLevelStorage(const std::string& a, const std::string& path) :
	field_8(a),
	m_levelDirPath(path)
{
	createFolderIfNotExists(m_levelDirPath.c_str());

	std::string datLevel  = m_levelDirPath + "/" + "level.dat";
	std::string datPlayer = m_levelDirPath + "/" + "player.dat";

	m_pLevelData = new LevelData;
	if (!readLevelData(datLevel, m_pLevelData))
	{
		delete m_pLevelData;
		m_pLevelData = nullptr;
		return;
	}

	readPlayerData(datPlayer, m_pLevelData);
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

void ExternalFileLevelStorage::saveLevelData(LevelData* levelData, std::vector<Player*>& players)
{
	writeLevelData(m_levelDirPath + "/" + "level.dat", levelData);
	savePlayerData(levelData, players);

	SAFE_DELETE(m_pLevelData);

	m_pLevelData = new LevelData(*levelData);
}

void ExternalFileLevelStorage::savePlayerData(LevelData* levelData, std::vector<Player*>& players)
{
	if (players.empty())
		return;

	FILE* pFile = fopen((m_levelDirPath + "/" + "player.dat").c_str(), "wb");
	if (!pFile)
	{
		LogMsg("Not saving player data");
		return;
	}

	levelData->m_LocalPlayerData.savePlayer(players[0]);

	int nPlayers = 1;
	fwrite(&nPlayers, sizeof nPlayers, 1, pFile);

	int nSizePD = 80;
	fwrite(&nSizePD,  sizeof nSizePD, 1, pFile);

	// @NOTE: No reason to swap elementCount and elementSize here. I understood it the
	// last time - to check whether the data loaded all the way. However, no checks are
	// done here.
	fwrite(&levelData->m_LocalPlayerData, 1, nSizePD, pFile);

	fclose(pFile);
}

void ExternalFileLevelStorage::closeAll()
{
}

void ExternalFileLevelStorage::tick()
{
}

void ExternalFileLevelStorage::flush()
{
}

LevelChunk* ExternalFileLevelStorage::load(Level* level, int x, int z)
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
	if (!m_pRegionFile->readChunk(x, z, &pBitStream))
		return nullptr;

	pBitStream->ResetReadPointer();

	TileID* pData = new TileID[16 * 16 * 128];
	pBitStream->Read((char*)pData, 16 * 16 * 128 * sizeof(TileID));

	LevelChunk* pChunk = new LevelChunk(level, pData, x, z);
	pBitStream->Read((char*)pChunk->m_tileData, 16 * 16 * 128 / 2);

	if (m_pLevelData->getVersion() == 1)
	{
		pBitStream->Read((char*)pChunk->m_lightSky, 16 * 16 * 128 / 2);
		pBitStream->Read((char*)pChunk->m_lightBlk, 16 * 16 * 128 / 2);
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

void ExternalFileLevelStorage::save(Level* level, LevelChunk* chunk)
{
	if (!m_pRegionFile)
		m_pRegionFile = new RegionFile(m_levelDirPath);

	if (!m_pRegionFile->open())
	{
		SAFE_DELETE(m_pRegionFile);
		m_pRegionFile = nullptr;

		LogMsg("Not saving :(   (x: %d  z: %d)", chunk->m_chunkX, chunk->m_chunkZ);
		return;
	}

	RakNet::BitStream bs;
	bs.Write((const char*)chunk->m_pBlockData, 16 * 16 * 128 * sizeof(TileID));
	bs.Write((const char*)chunk->m_tileData,   16 * 16 * 128 / 2);

	if (m_pLevelData->field_20 == 1)
	{
		bs.Write((const char*)chunk->m_lightSky, 16 * 16 * 128 / 2);
		bs.Write((const char*)chunk->m_lightBlk, 16 * 16 * 128 / 2);
	}

	bs.Write((const char*)chunk->m_updateMap, sizeof chunk->m_updateMap);

	m_pRegionFile->writeChunk(chunk->m_chunkX, chunk->m_chunkZ, bs);
}

void ExternalFileLevelStorage::saveEntities(Level* level, LevelChunk* chunk)
{
	// no op
}

bool ExternalFileLevelStorage::readLevelData(const std::string& path, LevelData* pLevelData)
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
		SAFE_DELETE(data);
		goto _cleanup;
	}

	RakNet::BitStream bs(data, length, false);
	pLevelData->read(bs, version);

	SAFE_DELETE_ARRAY(data);
	fclose(pFile);

	return true;
}

bool ExternalFileLevelStorage::readPlayerData(const std::string& path, LevelData* pLevelData)
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

	if (fread(&pLevelData->m_LocalPlayerData, 1, sizeof pLevelData->m_LocalPlayerData, pFile) == size)
		pLevelData->m_nPlayers = nPlayers;

	fclose(pFile);
	return true;

_cleanup:
	fclose(pFile);
	return false;
}
