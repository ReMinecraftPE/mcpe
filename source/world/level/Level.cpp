/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"

#include <algorithm>
#include "common/Util.hpp"
#include "world/level/levelgen/chunk/ChunkCache.hpp"
#include "Explosion.hpp"
#include "Region.hpp"

Level::Level(LevelStorage* pStor, const std::string& str, TLong seed, int x, Dimension *pDimension)
{
	m_bInstantTicking = false;
	m_bIsMultiplayer = false;
	m_bPostProcessing = false;
	m_skyDarken = 0;
	field_30 = 0;
	m_pDimension = nullptr;
	m_bCalculatingInitialSpawn = false;
	m_pChunkSource = nullptr;
	m_pLevelStorage = pStor;
	field_AA8 = 42184323;
	field_AAC = 1013904223;
	m_bUpdateLights = true;
	field_B08 = 0;
	field_B0C = 0;

	m_random.setSeed(1); // initialize with a seed of 1

	LevelData* pData = m_pLevelStorage->prepareLevel(this);

	field_B0C = pData == 0;

	// @BUG: leaking a Dimension*?
	if (pDimension)
		m_pDimension = pDimension;
	else
		m_pDimension = new Dimension;

	if (!pData)
		m_levelData = LevelData(seed, str, x);
	else
		m_levelData = *pData;

	m_pDimension->init(this);

	m_pPathFinder = new PathFinder();

	m_pChunkSource = createChunkSource();
	updateSkyBrightness();
}

Level::~Level()
{
	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pDimension);
	SAFE_DELETE(m_pPathFinder);

	const size_t size = m_entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* pEnt = m_entities.at(i);
		
		//you better HOPE this is freed by Minecraft!
		//Really should have used shared pointers and stuff.
		if (!pEnt->isLocalPlayer())
			delete pEnt;
	}
	
	m_entities.clear();
}
ChunkSource* Level::createChunkSource()
{
	if (m_pLevelStorage)
	{
		ChunkStorage* pChkStr = m_pLevelStorage->createChunkStorage(m_pDimension);
		ChunkSource* pChkSrc = m_pDimension->createRandomLevelSource();
		return new ChunkCache(this, pChkStr, pChkSrc);
	}

	LOG_I("No level data, calling dimension->createRandomLevelSource");
	return m_pDimension->createRandomLevelSource();
}

float Level::getTimeOfDay(float f)
{
	return m_pDimension->getTimeOfDay(m_levelData.field_10, f);
}

int Level::getSkyDarken(float f)
{
	float x = Mth::cos(getSunAngle(f));
	float y = 1.0f - (2 * x + 0.5f);

	if (y < 0.0f)
		return 0; // no darken
	if (y > 1.0f)
		return 11; // full dark

	return int(y * 11.0f);
}

void Level::updateSkyBrightness()
{
	int skyDarken = getSkyDarken(1.0f);
	if (m_skyDarken != skyDarken)
		m_skyDarken  = skyDarken;
}

LevelStorage* Level::getLevelStorage()
{
	return m_pLevelStorage;
}

LevelData* Level::getLevelData()
{
	return &m_levelData;
}

BiomeSource* Level::getBiomeSource()
{
	return m_pDimension->m_pBiomeSource;
}

ChunkSource* Level::getChunkSource()
{
	return m_pChunkSource;
}

LevelChunk* Level::getChunk(int x, int z)
{
	return getChunkSource()->getChunk(x, z);
}

TileID Level::getTile(int x, int y, int z)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);
	return pChunk->getTile(x & 0xF, y, z & 0xF);
}

int Level::getData(int x, int y, int z)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);
	return pChunk->getData(x & 0xF, y, z & 0xF);
}

int Level::getBrightness(const LightLayer& ll, int x, int y, int z)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return ll.m_x;

	if (!hasChunk(x >> 4, z >> 4))
		return 0;

	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);
	return pChunk->getBrightness(ll, x & 0xF, y, z & 0xF);
}

float Level::getBrightness(int x, int y, int z)
{
	return m_pDimension->field_10[getRawBrightness(x, y, z)];
}

int Level::getRawBrightness(int x, int y, int z)
{
	return getRawBrightness(x, y, z, true);
}

int Level::getRawBrightness(int x, int y, int z, bool b)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z)
		return 15;

	// this looks like some kind of hack.
	if (b && (getTile(x, y, z) == Tile::stoneSlabHalf->m_ID || getTile(x, y, z) == Tile::farmland->m_ID))
	{
		int b1 = getRawBrightness(x, y + 1, z, false);
		int b2 = getRawBrightness(x + 1, y, z, false);
		int b3 = getRawBrightness(x - 1, y, z, false);
		int b4 = getRawBrightness(x, y, z + 1, false);
		int b5 = getRawBrightness(x, y, z - 1, false);

		if (b2 < b1) b2 = b1;
		if (b3 < b2) b3 = b2;
		if (b4 < b3) b4 = b3;
		if (b5 < b4) b5 = b4;

		return b5;
	}

	if (y < C_MIN_Y)
		return 0;

	if (y >= C_MAX_Y)
	{
		int r = 15 - m_skyDarken;
		if (r < 0)
			r = 0;

		return r;
	}

	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);
	return pChunk->getRawBrightness(x & 0xF, y, z & 0xF, m_skyDarken);
}

int Level::getSeaLevel()
{
	return 63;
}

int Level::getSeed()
{
	return m_levelData.m_seed;
}

TLong Level::getTime()
{
	return m_levelData.field_10;
}

void Level::setTime(TLong time)
{
	m_levelData.field_10 = time;
}

void Level::swap(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int tile1 = getTile(x1, y1, z1);
	int data1 = getData(x1, y1, z1);
	int tile2 = getTile(x2, y2, z2);
	int data2 = getData(x2, y2, z2);

	setTileAndDataNoUpdate(x1, y1, z1, tile2, data2);
	setTileAndDataNoUpdate(x2, y2, z2, tile1, data1);

	updateNeighborsAt(x1, y1, z1, tile2);
	updateNeighborsAt(x2, y2, z2, tile1);
}

bool Level::isDay()
{
	return m_skyDarken <= 3;
}

bool Level::isEmptyTile(int x, int y, int z)
{
	return getTile(x, y, z) == 0;
}

bool Level::isSolidTile(int x, int y, int z)
{
	Tile* pTile = Tile::tiles[getTile(x, y, z)];
	if (!pTile) return false;

	return pTile->isSolidRender();
}

Material* Level::getMaterial(int x, int y, int z)
{
	Tile* pTile = Tile::tiles[getTile(x, y, z)];
	if (!pTile) return Material::air;

	return pTile->m_pMaterial;
}

Entity* Level::getEntity(int id)
{
	// prioritize players first.
	for (std::vector<Player*>::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Player* pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}
	for (std::vector<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}

	return nullptr;
}

EntityVector* Level::getAllEntities()
{
	return &m_entities;
}

bool Level::hasChunk(int x, int z)
{
	return m_pChunkSource->hasChunk(x, z);
}

EntityVector* Level::getEntities(Entity* pEntExclude, const AABB& aabb)
{
	m_getEntitiesResult.clear();

	int lowerXBound = Mth::floor((aabb.min.x - 2.0f) / 16);
	int lowerZBound = Mth::floor((aabb.min.z - 2.0f) / 16);
	int upperXBound = Mth::floor((aabb.max.x + 2.0f) / 16);
	int upperZBound = Mth::floor((aabb.max.z + 2.0f) / 16);

	for (int z = lowerZBound; z <= upperZBound; z++)
	{
		for (int x = lowerXBound; x <= upperXBound; x++)
		{
			if (!hasChunk(x, z)) continue;

			LevelChunk* pChunk = getChunk(x, z);
			pChunk->getEntities(pEntExclude, aabb, m_getEntitiesResult);
		}
	}

	return &m_getEntitiesResult;
}

void Level::setUpdateLights(bool b)
{
	m_bUpdateLights = b;
}

bool Level::updateLights()
{
	// if more than 49 concurrent updateLights() calls?
	if (field_B08 > 49)
		return false;

	field_B08++;

	if (m_lightUpdates.empty())
	{
		field_B08--;
		return false;
	}

	for (int i = 499; i; i--)
	{
		LightUpdate lu = *(m_lightUpdates.end() - 1);
		m_lightUpdates.pop_back();

		lu.update(this);

		if (m_lightUpdates.empty())
		{
			field_B08--;
			return false;
		}
	}

	field_B08--;
	return true;
}

bool Level::hasChunksAt(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
{
	if (minY >= C_MAX_Y || maxY < 0)
		return false;

	int minChX = minX >> 4, minChZ = minZ >> 4;
	int maxChX = maxX >> 4, maxChZ = maxZ >> 4;
	for (int chX = minChX; chX <= maxChX; chX++)
	{
		for (int chZ = minChZ; chZ <= maxChZ; chZ++)
		{
			if (!hasChunk(chX, chZ))
				return false;
		}
	}

	return true;
}

bool Level::hasChunksAt(int x, int y, int z, int rad)
{
	return hasChunksAt(x - rad, y - rad, z - rad, x + rad, y + rad, z + rad);
}

void Level::setBrightness(const LightLayer& ll, int x, int y, int z, int bright)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return;

	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);
	pChunk->setBrightness(ll, x & 0xF, y, z & 0xF, bright);

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileBrightnessChanged(x, y, z);
	}
}

int Level::getDirectSignal(int x, int y, int z, int dir)
{
	TileID tile = getTile(x, y, z);
	if (!tile) return 0;

	return Tile::tiles[tile]->getDirectSignal(this, x, y, z, dir);
}

int Level::getSignal(int x, int y, int z, int dir)
{
	if (isSolidTile(x, y, z))
		return hasDirectSignal(x, y, z);

	TileID tile = getTile(x, y, z);
	if (!tile) return 0;

	return Tile::tiles[tile]->getSignal(this, x, y, z, dir);
}

bool Level::hasDirectSignal(int x, int y, int z)
{
	if (getDirectSignal(x, y - 1, z, DIR_YNEG)) return true;
	if (getDirectSignal(x, y + 1, z, DIR_YPOS)) return true;
	if (getDirectSignal(x, y, z - 1, DIR_ZNEG)) return true;
	if (getDirectSignal(x, y, z + 1, DIR_ZPOS)) return true;
	if (getDirectSignal(x - 1, y, z, DIR_XNEG)) return true;
	if (getDirectSignal(x + 1, y, z, DIR_XPOS)) return true;
	return false;
}

bool Level::hasNeighborSignal(int x, int y, int z)
{
	if (getSignal(x, y - 1, z, DIR_YNEG)) return true;
	if (getSignal(x, y + 1, z, DIR_YPOS)) return true;
	if (getSignal(x, y, z - 1, DIR_ZNEG)) return true;
	if (getSignal(x, y, z + 1, DIR_ZPOS)) return true;
	if (getSignal(x - 1, y, z, DIR_XNEG)) return true;
	if (getSignal(x + 1, y, z, DIR_XPOS)) return true;
	return false;
}

bool Level::hasChunkAt(int gx, int gy, int gz)
{
	return hasChunk(gx >> 4, gz >> 4);
}

LevelChunk* Level::getChunkAt(int gx, int gz)
{
	return getChunk(gx >> 4, gz >> 4);
}

void Level::updateLight(const LightLayer& ll, int a, int b, int c, int d, int e, int f, bool unimportant)
{
	static int nUpdateLevels;

	if ((m_pDimension->field_E && &ll == &LightLayer::Sky) || !m_bUpdateLights)
		return;

	nUpdateLevels++;
	if (nUpdateLevels == 50)
	{
		nUpdateLevels--;
		return;
	}

	if (!hasChunkAt((d + a) / 2, 64, (f + c) / 2) || getChunkAt((d + a) / 2, (f + c) / 2)->isEmpty())
	{
		nUpdateLevels--;
		return;
	}

	size_t size = m_lightUpdates.size();
	if (unimportant)
	{
		size_t count = 5;
		if (count > size)
			count = size;

		for (size_t i = 0; i < count; i++)
		{
			LightUpdate& update = m_lightUpdates[size - i - 1];
			if (update.m_lightLayer == &ll && update.expandToContain(a, b, c, d, e, f))
			{
				nUpdateLevels--;
				return;
			}
		}
	}

	m_lightUpdates.push_back(LightUpdate(ll, a, b, c, d, e, f));

	// huh??
	if (m_lightUpdates.size() > 1000000)
		m_lightUpdates.clear();

	nUpdateLevels--;
}

void Level::updateLight(const LightLayer& ll, int a, int b, int c, int d, int e, int f)
{
	updateLight(ll, a, b, c, d, e, f, true);
}

void Level::updateLightIfOtherThan(const LightLayer& ll, int x, int y, int z, int bright)
{
	if (m_pDimension->field_E && &ll == &LightLayer::Sky)
		return;

	if (!hasChunkAt(x, y, z))
		return;

	if (&ll == &LightLayer::Sky)
	{
		if (isSkyLit(x, y, z))
			bright = 15;
	}
	else if (&ll == &LightLayer::Block)
	{
		TileID tile = getTile(x, y, z);
		if (bright < Tile::lightEmission[tile])
			bright = Tile::lightEmission[tile];
	}

	int oldbr = getBrightness(ll, x, y, z);
	if (bright != oldbr)
	{
		updateLight(ll, x, y, z, x, y, z);
	}
}

bool Level::isSkyLit(int x, int y, int z)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y)
		// there's nothing out there!
		return false;

	if (y >= C_MAX_Y)
		return true;

	if (!hasChunk(x >> 4, z >> 4))
		return false;

	return getChunk(x >> 4, z >> 4)->isSkyLit(x & 0xF, y, z & 0xF);
}

bool Level::setTileAndDataNoUpdate(int x, int y, int z, TileID tile, int data)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(x >> 4, z >> 4))
		return false;

	return getChunk(x >> 4, z >> 4)->setTileAndData(x & 0xF, y, z & 0xF, tile, data);
}

int Level::getHeightmap(int x, int z)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z)
		// there's nothing out there!
		return 0;

	if (!hasChunk(x >> 4, z >> 4))
		return 0;

	return getChunk(x >> 4, z >> 4)->getHeightmap(x & 0xF, z & 0xF);
}

void Level::lightColumnChanged(int x, int z, int y1, int y2)
{
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	setTilesDirty(x, y1, z, x, y2, z);
}

bool Level::setDataNoUpdate(int x, int y, int z, int data)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(x >> 4, z >> 4))
		return false;

	LevelChunk* pChk = getChunk(x >> 4, z >> 4);
	if (pChk->getData(x & 0xF, y, z & 0xF) == data)
		return false; // no update

	pChk->setData(x & 0xF, y, z & 0xF, data);
	return true;
}

bool Level::setTileNoUpdate(int x, int y, int z, TileID tile)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (x < C_MIN_X || z < C_MIN_Z || x >= C_MAX_X || z > C_MAX_Z || y < C_MIN_Y || y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(x >> 4, z >> 4))
		return false;

	return getChunk(x >> 4, z >> 4)->setTile(x & 0xF, y, z & 0xF, tile);
}

void Level::sendTileUpdated(int x, int y, int z)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileChanged(x, y, z);
	}
}

void Level::neighborChanged(int x, int y, int z, TileID tile)
{
	if (field_30 || m_bIsMultiplayer) return;

	Tile* pTile = Tile::tiles[getTile(x, y, z)];
	if (pTile)
		pTile->neighborChanged(this, x, y, z, tile);
}

void Level::updateNeighborsAt(int x, int y, int z, TileID tile)
{
	neighborChanged(x - 1, y, z, tile);
	neighborChanged(x + 1, y, z, tile);
	neighborChanged(x, y - 1, z, tile);
	neighborChanged(x, y + 1, z, tile);
	neighborChanged(x, y, z - 1, tile);
	neighborChanged(x, y, z + 1, tile);
}

void Level::tileUpdated(int x, int y, int z, TileID tile)
{
	sendTileUpdated(x, y, z);
	updateNeighborsAt(x, y, z, tile);
}

bool Level::setTileAndData(int x, int y, int z, TileID tile, int data)
{
	if (setTileAndDataNoUpdate(x, y, z, tile, data))
	{
		tileUpdated(x, y, z, tile);
		return true;
	}
	return false;
}

bool Level::setData(int x, int y, int z, int data)
{
	if (setDataNoUpdate(x, y, z, data))
	{
		tileUpdated(x, y, z, getTile(x, y, z));
		return true;
	}
	return false;
}

bool Level::setTile(int x, int y, int z, TileID tile)
{
	if (setTileNoUpdate(x, y, z, tile))
	{
		tileUpdated(x, y, z, tile);
		return true;
	}
	return false;
}

void Level::setTilesDirty(int x1, int y1, int z1, int x2, int y2, int z2)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->setTilesDirty(x1, y1, z1, x2, y2, z2);
	}
}

void Level::entityAdded(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityAdded(pEnt);
	}
}

void Level::entityRemoved(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityRemoved(pEnt);
	}
}

AABBVector* Level::getCubes(const Entity* pEntUnused, const AABB& aabb)
{
	m_aabbs.clear();

	int lowerX = Mth::floor(aabb.min.x);
	int upperX = Mth::floor(aabb.max.x + 1);
	int lowerY = Mth::floor(aabb.min.y);
	int upperY = Mth::floor(aabb.max.y + 1);
	int lowerZ = Mth::floor(aabb.min.z);
	int upperZ = Mth::floor(aabb.max.z + 1);

	for (int x = lowerX; x <= upperX; x++)
	{
		for (int z = lowerZ; z <= upperZ; z++)
		{
			if (!hasChunkAt(x, 64, z)) continue;

			for (int y = lowerY; y <= upperY; y++)
			{
				Tile* pTile = Tile::tiles[getTile(x, y, z)];
				if (pTile)
					pTile->addAABBs(this, x, y, z, &aabb, m_aabbs);
			}
		}
	}

	return &m_aabbs;
}

std::vector<LightUpdate>* Level::getLightsToUpdate()
{
	return &m_lightUpdates;
}

Entity* Level::getNearestPlayer(Entity* entity, float f)
{
	return getNearestPlayer(entity->m_pos.x, entity->m_pos.y, entity->m_pos.z, f);
}

Entity* Level::getNearestPlayer(float x, float y, float z, float maxDist)
{
	float dist = -1.0f;
	Player* pPlayer = nullptr;

	for (std::vector<Player*>::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Player* player = *it;
		float ldist = player->distanceToSqr(x, y, z);
		if ((maxDist < 0.0f || ldist < maxDist * maxDist) && (dist == -1.0f || dist > ldist))
		{
			pPlayer = player;
			dist = ldist;
		}
	}

	return pPlayer;
}

bool Level::containsFireTile(AABB aabb)
{
	int minX = Mth::floor(aabb.min.x),     minY = Mth::floor(aabb.min.y),     minZ = Mth::floor(aabb.min.z),
		maxX = Mth::floor(aabb.max.x + 1), maxY = Mth::floor(aabb.max.y + 1), maxZ = Mth::floor(aabb.max.z + 1);

	if (!hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return false;
	
	for (int x = minX; x < maxX; x++)
		for (int y = minY; y < maxY; y++)
			for (int z = minZ; z < maxZ; z++)
			{
				TileID tileID = getTile(x, y, z);
				
				if (tileID == Tile::fire->m_ID || tileID == Tile::lava->m_ID || tileID == Tile::calmLava->m_ID)
					return true;
			}

	return false;
}

bool Level::containsAnyLiquid(AABB aabb)
{
	int minX = Mth::floor(aabb.min.x),     minY = Mth::floor(aabb.min.y),     minZ = Mth::floor(aabb.min.z),
		maxX = Mth::floor(aabb.max.x + 1), maxY = Mth::floor(aabb.max.y + 1), maxZ = Mth::floor(aabb.max.z + 1);

	if (!hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return false;
	
	for (int x = minX; x < maxX; x++)
		for (int y = minY; y < maxY; y++)
			for (int z = minZ; z < maxZ; z++)
			{
				TileID tileID = getTile(x, y, z);
				
				if (Tile::tiles[tileID])
				{
					if (Tile::tiles[tileID]->m_pMaterial->isLiquid())
						return true;
				}
			}

	return false;
}

bool Level::containsLiquid(const AABB& aabb, const Material* pMtl)
{
	int minX = Mth::floor(aabb.min.x),     minY = Mth::floor(aabb.min.y),     minZ = Mth::floor(aabb.min.z),
		maxX = Mth::floor(aabb.max.x + 1), maxY = Mth::floor(aabb.max.y + 1), maxZ = Mth::floor(aabb.max.z + 1);

	if (!hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return false;

	for (int x = minX; x < maxX; x++)
		for (int y = minY; y < maxY; y++)
			for (int z = minZ; z < maxZ; z++)
			{
				TileID tileID = getTile(x, y, z);

				if (!Tile::tiles[tileID] || Tile::tiles[tileID]->m_pMaterial != pMtl)
					continue;

				int data = getData(x, y, z);
				
				float height;
				if (data <= 7)
					height = (float(y) + 1.0f) + float(data) / 8.0f;
				else
					height = float(y + 1);

				if (aabb.min.y <= height)
					return true;
			}

	return false;
}

bool Level::containsMaterial(const AABB& aabb, const Material* pMtl)
{
	int minX = Mth::floor(aabb.min.x),     minY = Mth::floor(aabb.min.y),     minZ = Mth::floor(aabb.min.z),
		maxX = Mth::floor(aabb.max.x + 1), maxY = Mth::floor(aabb.max.y + 1), maxZ = Mth::floor(aabb.max.z + 1);

	if (!hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return false;
	
	for (int x = minX; x < maxX; x++)
		for (int y = minY; y < maxY; y++)
			for (int z = minZ; z < maxZ; z++)
			{
				TileID tileID = getTile(x, y, z);
				
				if (Tile::tiles[tileID] && Tile::tiles[tileID]->m_pMaterial == pMtl)
					return true;
			}

	return false;
}

bool Level::checkAndHandleWater(const AABB& aabb, const Material* pMtl, Entity* pEnt)
{
	int minX = Mth::floor(aabb.min.x),     minY = Mth::floor(aabb.min.y),     minZ = Mth::floor(aabb.min.z),
		maxX = Mth::floor(aabb.max.x + 1), maxY = Mth::floor(aabb.max.y + 1), maxZ = Mth::floor(aabb.max.z + 1);

	if (!hasChunksAt(minX, minY, minZ, maxX, maxY, maxZ))
		return false;

	Vec3 v;
	
	bool bInWater = false;
	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY + 1; y < maxY; y++)
		{
			int y2 = y - 1;
			for (int z = minZ; z < maxZ; z++)
			{
				Tile* pTile = Tile::tiles[getTile(x, y2, z)];
				if (!pTile || pTile->m_pMaterial != pMtl)
					continue;

				int data = getData(x, y2, z);
				int level = data <= 7 ? data + 1 : 1;
				if (float(maxY) >= float(y) - float(level) / 9.0f)
				{
					pTile->handleEntityInside(this, x, y2, z, pEnt, v);
					bInWater = true;
				}
			}
		}
	}

	if (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z) > 0.0f)
	{
		pEnt->m_vel.x += v.x * 0.004f;
		pEnt->m_vel.y += v.y * 0.004f;
		pEnt->m_vel.z += v.z * 0.004f;
	}

	return bInWater;
}

Pos Level::getSharedSpawnPos()
{
	return m_levelData.m_spawnPos;
}

TileID Level::getTopTile(int x, int z)
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(x, y + 1, z); y++);
	return getTile(x, y, z);
}

int Level::getTopTileY(int x, int z)
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(x, y + 1, z); y++);
	return y;
}

int Level::getTopSolidBlock(int x, int z)
{
	int y = C_MAX_Y - 1;
	LevelChunk* pChunk = getChunkAt(x, z);

	while (true)
	{
		if (!getMaterial(x, y, z)->blocksMotion())
			break;
		if (!y)
			return -1;
		y--;
	}

	int cx = x & 0xF, cz = z & 0xF;
	if (y <= C_MIN_Y)
		return 0;

	while (true)
	{
		TileID tile = pChunk->getTile(cx, y, cz);
		if (tile)
		{
			if (Tile::tiles[tile]->m_pMaterial->blocksMotion())
				return y + 1;
			
			if (Tile::tiles[tile]->m_pMaterial->isLiquid())
				break;
		}
		if (!--y)
			return -1;
	}

	return y + 1;
}

void Level::validateSpawn()
{
	if (m_levelData.m_spawnPos.y <= 0)
		m_levelData.m_spawnPos.y = C_MAX_Y / 2;

	int spawnX = m_levelData.m_spawnPos.x;
	int spawnZ = m_levelData.m_spawnPos.z;
#ifndef ORIGINAL_CODE
	int nAttempts = 0;
#endif

	// @TODO: fix these gotos
	if (getTopTile(spawnX, spawnZ))
		goto label_10;

	TileID tile;

	do
	{
		// While the spawn isn't valid
		do
		{
			spawnX += m_random.nextInt(8) - m_random.nextInt(8);
			spawnZ += m_random.nextInt(8) - m_random.nextInt(8);

			// avoid spawning near world border
			if (spawnX < 4) spawnX += 8;
			if (spawnZ < 4) spawnZ += 8;
			if (spawnX > C_MAX_CHUNKS_X * 16 - 4) spawnX -= 8;
			if (spawnZ > C_MAX_CHUNKS_Z * 16 - 4) spawnZ -= 8;

		#ifndef ORIGINAL_CODE
			nAttempts++;
			if (nAttempts >= 100000)
				goto _failure;
		#endif
		}
		while (!getTopTile(spawnX, spawnZ));

	label_10:
		tile = getTopTile(spawnX, spawnZ);
	}
	while (tile == Tile::invisible_bedrock->m_ID);

	m_levelData.m_spawnPos.x = spawnX;
	m_levelData.m_spawnPos.z = spawnZ;

#ifndef ORIGINAL_CODE
	return;

_failure:

	/*
	m_levelData.m_spawnPos.x = C_MAX_CHUNKS_X * 16 / 2;
	m_levelData.m_spawnPos.z = C_MAX_CHUNKS_X * 16 / 2;
	m_levelData.m_spawnPos.y = C_MAX_Y;
	*/

	m_levelData.m_spawnPos.x = 0;
	m_levelData.m_spawnPos.y = 32;
	m_levelData.m_spawnPos.z = 0;

	LOG_W("Failed to validate spawn point, using (%d, %d, %d)", m_levelData.m_spawnPos.x, m_levelData.m_spawnPos.y, m_levelData.m_spawnPos.z);

	return;
#endif
}

void Level::removeAllPendingEntityRemovals()
{
	Util::removeAll(m_entities, m_pendingEntityRemovals);

	for (EntityVector::iterator it = m_pendingEntityRemovals.begin(); it != m_pendingEntityRemovals.end(); it++)
	{
		Entity* ent = *it;
		ent->removed();

		if (hasChunk(ent->m_chunkX, ent->m_chunkZ))
			getChunk(ent->m_chunkX, ent->m_chunkZ)->removeEntity(ent);

		entityRemoved(ent);

		//@BUG: MEMORY LEAK -- probably leaking entities here?
	}

	m_pendingEntityRemovals.clear();
}

void Level::removeEntities(const EntityVector& vec)
{
	m_pendingEntityRemovals.insert(m_pendingEntityRemovals.end(), vec.begin(), vec.end());
}

bool Level::removeEntity(Entity* pEnt)
{
	pEnt->remove();

	if (pEnt->isPlayer())
		return Util::remove(m_players, (Player*)pEnt);

	return false;
}

bool Level::addEntity(Entity* pEnt)
{
	Entity* pOldEnt = getEntity(pEnt->hashCode());
	if (pOldEnt)
	{
		LOG_W("Entity %d already exists.", pEnt->hashCode());
		//removeEntity(pOldEnt);
	}

	if (!pEnt->isPlayer() && m_bIsMultiplayer)
	{
		LOG_W("Hey, why are you trying to add an non-player entity in a multiplayer world?");
	}

	//@NOTE: useless Mth::floor() calls
	Mth::floor(pEnt->m_pos.x / 16);
	Mth::floor(pEnt->m_pos.z / 16);

	//@NOTE: double check. Looks like someone just hacked it in
	//@BUG: Camera doesn't work. This might be a side effect of having a demo mode?
	//@BUG: Leaking the entity pointer.
#ifdef ORIGINAL_CODE
	if (!pEnt->isPlayer())
		return false;
#endif

	if (pEnt->isPlayer())
	{
		m_players.push_back((Player*)pEnt);
	}

	m_entities.push_back(pEnt);

	entityAdded(pEnt);

	return true;
}

void Level::loadPlayer(Player* player)
{
	if (!player) return;

	if (m_levelData.m_nPlayers == 1)
		m_levelData.m_LocalPlayerData.loadPlayer(player);

	addEntity(player);
}

void Level::prepare()
{
	while (m_pChunkSource->tick());
}

void Level::saveLevelData()
{
	m_pLevelStorage->saveLevelData(&m_levelData);
}

void Level::savePlayerData()
{
	m_pLevelStorage->savePlayerData(&m_levelData, m_players);
}

void Level::saveAllChunks()
{
	m_pChunkSource->saveAll();
}

#ifdef ENH_IMPROVED_SAVING
void Level::saveUnsavedChunks()
{
	m_pChunkSource->saveUnsaved();
}
#endif

void Level::setInitialSpawn()
{
	m_bCalculatingInitialSpawn = true;

	int spawnX = C_MAX_CHUNKS_X * 16 / 2, spawnZ = C_MAX_CHUNKS_Z * 16 / 2;
#ifndef ORIGINAL_CODE
	int nAttempts = 0;
#endif

	while (true)
	{
		if (m_pDimension->isValidSpawn(spawnX, spawnZ))
			break;

		spawnX += m_random.nextInt(32) - m_random.nextInt(32);
		spawnZ += m_random.nextInt(32) - m_random.nextInt(32);

		if (spawnX < 4) spawnX += 32;
		if (spawnZ < 4) spawnZ += 32;
		if (spawnX >= C_MAX_CHUNKS_X * 16 - 4) spawnX -= 32;
		if (spawnZ >= C_MAX_CHUNKS_Z * 16 - 4) spawnZ -= 32;

#ifndef ORIGINAL_CODE
		if (++nAttempts > 100000)
			goto _failure;
#endif
	}

	m_levelData.m_spawnPos.x = spawnX;
	m_levelData.m_spawnPos.z = spawnZ;
	m_levelData.m_spawnPos.y = 64;

	m_bCalculatingInitialSpawn = false;

#ifndef ORIGINAL_CODE
	return;

_failure:

	/*
	m_levelData.m_spawnPos.x = C_MAX_CHUNKS_X * 16 / 2;
	m_levelData.m_spawnPos.z = C_MAX_CHUNKS_X * 16 / 2;
	m_levelData.m_spawnPos.y = C_MAX_Y;
	*/

	m_levelData.m_spawnPos.x = 0;
	m_levelData.m_spawnPos.y = 32;
	m_levelData.m_spawnPos.z = 0;

	LOG_W("Failed to validate spawn point, using (%d, %d, %d)", m_levelData.m_spawnPos.x, m_levelData.m_spawnPos.y, m_levelData.m_spawnPos.z);

	return;
#endif
}

void Level::setSpawnPos(Pos pos)
{
	m_levelData.m_spawnPos = pos;
}

void Level::setSpawnSettings(bool a, bool b)
{
}

bool Level::canSeeSky(int x, int y, int z)
{
	LevelChunk* pChunk = getChunk(x >> 4, z >> 4);

	//@BUG: no nullptr check
#ifndef ORIGINAL_CODE
	if (!pChunk)
		return true;
#endif

	return pChunk->isSkyLit(x & 0xF, y, z & 0xF);
}

Vec3 Level::getSkyColor(Entity* pEnt, float f)
{
	Vec3 result;

	float fTODCosAng = Mth::cos(getSunAngle(f));

	// @TODO: Fix the gotos

	result.z = 2 * fTODCosAng + 0.5f;
	if (result.z < 0.0f)
		result.z = 0.0f;
	if (result.z > 1.0f)
		result.z = 1.0f;

	// @NOTE: Unused result. In JE, it tries to get the biome that the player is standing in.
	Mth::floor(pEnt->m_pos.x);
	Mth::floor(pEnt->m_pos.z);

	result.x = result.z * 0.6f;
	result.y = result.x;

	return result;
}

Vec3 Level::getFogColor(float f)
{
	return m_pDimension->getFogColor(getTimeOfDay(f), f);
}

Vec3 Level::getCloudColor(float f)
{
	Vec3 result;

	float fTODCosAng = Mth::cos(getSunAngle(f));

	float mult = 2 * fTODCosAng + 0.5f;
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	result.x = mult * 0.9f + 0.1f;
	result.y = result.x;
	result.z = mult * 0.85f + 0.15f;

	return result;
}

bool Level::isUnobstructed(AABB* aabb)
{
	EntityVector* entities = getEntities(nullptr, *aabb);
	if (entities->size() <= 0)
		return true;

	for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->m_bRemoved)
			continue;

		if (!pEnt->field_34)
			continue;

		return false;
	}

	return true;
}

bool Level::mayInteract(Player* player, int x, int y, int z)
{
	return true;
}

bool Level::mayPlace(TileID tile, int x, int y, int z, bool b)
{
	TileID oldTile = getTile(x, y, z);

	Tile *pTile = Tile::tiles[tile], *pOldTile = Tile::tiles[oldTile];

	AABB* aabb = pTile->getAABB(this, x, y, z);

	if (!b && aabb && !isUnobstructed(aabb))
		return false;

	if (pOldTile == Tile::water     ||
		pOldTile == Tile::calmWater ||
		pOldTile == Tile::lava      ||
		pOldTile == Tile::calmLava  ||
		pOldTile == Tile::fire      ||
		pOldTile == Tile::topSnow)
		return true;

	if (pOldTile || tile <= 0)
		return false;

	return pTile->mayPlace(this, x, y, z);
}

void Level::removeListener(LevelListener* listener)
{
	std::vector<LevelListener*>::iterator iter = std::find(m_levelListeners.begin(), m_levelListeners.end(), listener);
	if (iter != m_levelListeners.end())
		m_levelListeners.erase(iter);
}

void Level::addListener(LevelListener* listener)
{
	m_levelListeners.push_back(listener);
}

void Level::tickPendingTicks(bool b)
{
	int size = 10000; // note: 65,536 in Minecraft Java
	if (size > int(m_pendingTicks.size()))
		size = int(m_pendingTicks.size());

	for (int i = 0; i < size; i++)
	{
		const TickNextTickData& t = *m_pendingTicks.begin();
		if (!b && t.m_delay > m_levelData.field_10)
			break;

		if (hasChunksAt(t.field_4 - 8, t.field_8 - 8, t.field_C - 8, t.field_4 + 8, t.field_8 + 8, t.field_C + 8))
		{
			TileID tile = getTile(t.field_4, t.field_8, t.field_C);
			if (tile == t.field_10 && tile > 0)
				Tile::tiles[tile]->tick(this, t.field_4, t.field_8, t.field_C, &m_random);
		}

		m_pendingTicks.erase(m_pendingTicks.begin());
	}
}

void Level::tickTiles()
{
	m_chunksToUpdate.clear();

	for (std::vector<Player*>::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Player* player = *it;

		int chkX = Mth::floor(player->m_pos.x / 16.0f);
		int chkZ = Mth::floor(player->m_pos.z / 16.0f);

		for (int x = -C_TICK_DISTANCE_CHKS; x <= C_TICK_DISTANCE_CHKS; x++)
		{
			for (int z = -C_TICK_DISTANCE_CHKS; z <= C_TICK_DISTANCE_CHKS; z++)
			{
				m_chunksToUpdate.insert(ChunkPos(chkX + x, chkZ + z));
			}
		}
	}

	for (std::set<ChunkPos>::iterator it = m_chunksToUpdate.begin(); it != m_chunksToUpdate.end(); it++)
	{
		ChunkPos pos = *it;
		LevelChunk* pChunk = getChunk(pos.x, pos.z);

		int globX = pos.x * 16, globZ = pos.z * 16;

		for (int i = 0; i < 80; i++)
		{
			field_AA8 = 3 * field_AA8 + field_AAC;

			int tileX = (field_AA8 >> 2) & 0xF;
			int tileZ = (field_AA8 >> 10) & 0xF;
			int tileY = (field_AA8 >> 18) & 0x7F;

			TileID tile = pChunk->getTile(tileX, tileY, tileZ);
			if (Tile::shouldTick[tile])
				Tile::tiles[tile]->tick(this, tileX + globX, tileY, tileZ + globZ, &m_random);
		}
	}
}

void Level::tick(Entity* pEnt, bool b)
{
	int tileX = Mth::floor(pEnt->m_pos.x), tileZ = Mth::floor(pEnt->m_pos.z);

	if (b)
	{
		if (!hasChunksAt(tileX - 32, 0, tileZ - 32, tileX + 32, 128, tileZ + 32))
		{
			LOG_W("Not updating entity %d because we don't have chunks around it loaded", pEnt->m_EntityID);
			return;
		}

		pEnt->field_98 = pEnt->m_pos;
		pEnt->field_5C = pEnt->m_yaw;
		pEnt->field_60 = pEnt->m_pitch;

		if (pEnt->m_bInAChunk)
			pEnt->tick();
	}
	else
	{
		pEnt->field_98 = pEnt->m_pos;
		pEnt->field_5C = pEnt->m_yaw;
		pEnt->field_60 = pEnt->m_pitch;
	}

	int chunkX = Mth::floor(pEnt->m_pos.x / 16);
	int chunkY = Mth::floor(pEnt->m_pos.y / 16);
	int chunkZ = Mth::floor(pEnt->m_pos.z / 16);

	if (!pEnt->m_bInAChunk || chunkX != pEnt->m_chunkX || chunkY != pEnt->m_chunkY || chunkZ != pEnt->m_chunkZ)
	{
		// move the entity to the new chunk
		if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkX, pEnt->m_chunkZ))
		{
			getChunk(pEnt->m_chunkX, pEnt->m_chunkZ)->removeEntity(pEnt);
		}

		if (hasChunk(chunkX, chunkZ))
		{
			pEnt->m_bInAChunk = true;
			getChunk(chunkX, chunkZ)->addEntity(pEnt);
		}
		else
		{
			pEnt->m_bInAChunk = false;
		}
	}

}

void Level::tick(Entity* pEnt)
{
	tick(pEnt, true);
}

int TICKSPERSECOND = 0;
int LASTTICKED = 0;

void Level::tick()
{
	m_pChunkSource->tick();

#ifdef ENH_RUN_DAY_NIGHT_CYCLE
	int light = getSkyDarken(1.0f);
	if (light != m_skyDarken)
	{
		m_skyDarken = light;

		for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
		{
			LevelListener* pListener = *it;
			pListener->skyColorChanged();
		}
	}
#endif

	m_levelData.field_10++;

	tickPendingTicks(false);
	tickTiles();
}

void Level::tickEntities()
{
	// inlined in the original
	removeAllPendingEntityRemovals();

	for (int i = 0; i<int(m_entities.size()); i++)
	{
		Entity* pEnt = m_entities[i];
		tick(pEnt);

		if (!pEnt->m_bRemoved)
			continue;

		if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkX, pEnt->m_chunkZ))
			getChunk(pEnt->m_chunkX, pEnt->m_chunkZ)->removeEntity(pEnt);
		
		m_entities.erase(m_entities.begin() + i);
		i--;

		entityRemoved(pEnt);

		// If the entity isn't the local player (managed by Minecraft*), then delete it.
		if (!pEnt->isLocalPlayer())
			delete pEnt;
	}
}

HitResult Level::clip(Vec3 v1, Vec3 v2, bool flag)
{
	int v2xf = Mth::floor(v2.x);
	int v2yf = Mth::floor(v2.y);
	int v2zf = Mth::floor(v2.z);
	int v1xf = Mth::floor(v1.x);
	int v1yf = Mth::floor(v1.y);
	int v1zf = Mth::floor(v1.z);
	int counter = 200;
	while (counter-- >= 0)
	{
		if (v1xf == v2xf && v1yf == v2yf && v1zf == v2zf)
			break;

		float xd = 999.0f;
		float yd = 999.0f;
		float zd = 999.0f;
		if (v2xf > v1xf) xd = (float)v1xf + 1.0f;
		if (v2xf < v1xf) xd = (float)v1xf + 0.0f;
		if (v2yf > v1yf) yd = (float)v1yf + 1.0f;
		if (v2yf < v1yf) yd = (float)v1yf + 0.0f;
		if (v2zf > v1zf) zd = (float)v1zf + 1.0f;
		if (v2zf < v1zf) zd = (float)v1zf + 0.0f;
		float xe = 999.0f;
		float ye = 999.0f;
		float ze = 999.0f;
		float xl = v2.x - v1.x;
		float yl = v2.y - v1.y;
		float zl = v2.z - v1.z;
		if (xd != 999.0f) xe = (float)(xd - v1.x) / xl;
		if (yd != 999.0f) ye = (float)(yd - v1.y) / yl;
		if (zd != 999.0f) ze = (float)(zd - v1.z) / zl;
		int hitSide = 0;
		if (xe >= ye || xe >= ze)
		{
			if (ye >= ze)
			{
				hitSide = v2zf <= v1zf ? HitResult::MAXZ : HitResult::MINZ;
				v1.x = v1.x + (float)(xl * ze);
				v1.y = v1.y + (float)(yl * ze);
				v1.z = zd;
			}
			else
			{
				hitSide = (v2yf <= v1yf) ? HitResult::MAXY : HitResult::MINY;
				v1.x = v1.x + (float)(xl * ye);
				v1.y = yd;
				v1.z = v1.z + (float)(zl * ye);
			}
		}
		else
		{
			hitSide = v2xf <= v1xf ? HitResult::MAXX : HitResult::MINX;
			v1.x = xd;
			v1.y = v1.y + (float)(yl * xe);
			v1.z = v1.z + (float)(zl * xe);
		}

		Vec3 hitVec(v1.x, v1.y, v1.z);

		// Correct the hit positions for each vector
		hitVec.x = (float)Mth::floor(v1.x);
		v1xf = (int)hitVec.x;
		if (hitSide == HitResult::MAXX)
		{
			v1xf--;
			hitVec.x += 1.0;
		}


		hitVec.y = (float)Mth::floor(v1.y);
		v1yf = (int)hitVec.y;
		if (hitSide == HitResult::MAXY)
		{
			v1yf--;
			hitVec.y += 1.0;
		}


		hitVec.z = (float)Mth::floor(v1.z);
		v1zf = (int)hitVec.z;
		if (hitSide == HitResult::MAXZ)
		{
			v1zf--;
			hitVec.z += 1.0;
		}

		TileID tile = getTile(v1xf, v1yf, v1zf);
		int    data = getData(v1xf, v1yf, v1zf);
		Tile* pTile = Tile::tiles[tile];

		if (tile > 0 && pTile->mayPick(data, false))
		{
			HitResult hr = pTile->clip(this, v1xf, v1yf, v1zf, v1, v2);
			if (hr.isHit())
				return hr;
		}
	}

	return HitResult();
}

HitResult Level::clip(const Vec3& a, const Vec3& b)
{
	return clip(a, b, false);
}

void Level::addToTickNextTick(int a, int b, int c, int d, int delay)
{
	TickNextTickData tntd(a, b, c, d);
	if (m_bInstantTicking)
	{
		// @NOTE: Don't know why this check wasn't just placed at the beginning.
		if (!hasChunksAt(a - 8, b - 8, c - 8, a + 8, b + 8, c + 8))
			return;

		TileID tile = getTile(tntd.field_4, tntd.field_8, tntd.field_C);
		if (tile > 0 && tile == tntd.field_10)
			Tile::tiles[tntd.field_10]->tick(this, tntd.field_4, tntd.field_8, tntd.field_C, &m_random);
	}
	else
	{
		if (!hasChunksAt(a - 8, b - 8, c - 8, a + 8, b + 8, c + 8))
			return;

		if (d > 0)
			tntd.setDelay(delay + m_levelData.field_10);

		m_pendingTicks.insert(tntd);
	}
}

void Level::takePicture(TripodCamera* pCamera, Entity* pOwner)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->takePicture(pCamera, pOwner);
	}
}

void Level::addParticle(const std::string& name, float a, float b, float c, float d, float e, float f)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->addParticle(name, a, b, c, d, e, f);
	}
}

void Level::playSound(Entity* entity, const std::string& name, float volume, float pitch)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, entity->m_pos.x, entity->m_pos.y - entity->field_84, entity->m_pos.z, volume, pitch);
	}
}

void Level::playSound(float x, float y, float z, const std::string& name, float a, float b)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, x, y, z, a, b);
	}
}

void Level::animateTick(int x, int y, int z)
{
	Random random;

	for (int i = 0; i < 100; i++)
	{
		int x1, y1, z1;
		x1 = x + m_random.nextInt(16) - m_random.nextInt(16);
		y1 = y + m_random.nextInt(16) - m_random.nextInt(16);
		z1 = z + m_random.nextInt(16) - m_random.nextInt(16);
		TileID tile = getTile(x1, y1, z1);
		if (tile > 0)
			Tile::tiles[tile]->animateTick(this, x1, y1, z1, &random);
	}
}

float Level::getSeenPercent(Vec3 vec, AABB aabb)
{
	int raysTotal = 0, raysSeen = 0;

	float aabbSizeX = aabb.max.x - aabb.min.x;
	float aabbSizeY = aabb.max.y - aabb.min.y;
	float aabbSizeZ = aabb.max.z - aabb.min.z;

	// This shoots a bunch of rays from a point and checks if the rays hit something. Stuupiiiddd
	for (float xi = 0.0f; xi <= 1.0f; xi += 1.0f / (1.0f + 2 * aabbSizeX))
	{
		for (float yi = 0.0f; yi <= 1.0f; yi += 1.0f / (1.0f + 2 * aabbSizeY))
		{
			for (float zi = 0.0f; zi <= 1.0f; zi += 1.0f / (1.0f + 2 * aabbSizeZ))
			{
				Vec3 xvec;
				xvec.x = aabb.min.x + xi * aabbSizeX;
				xvec.y = aabb.min.y + yi * aabbSizeY;
				xvec.z = aabb.min.z + zi * aabbSizeZ;

				HitResult hr = clip(xvec, vec);
				if (hr.m_hitType == HitResult::NONE)
					raysSeen++;

				raysTotal++;
			}
		}
	}

	return float (raysSeen) / float (raysTotal);
}

void Level::explode(Entity* entity, float x, float y, float z, float power)
{
	explode(entity, x, y, z, power, false);
}

void Level::explode(Entity* entity, float x, float y, float z, float power, bool bIsFiery)
{
	Explosion expl(this, entity, x, y, z, power);
	expl.setFiery(bIsFiery);
	expl.explode();
	expl.addParticles();
}

void Level::addEntities(const std::vector<Entity*>& entities)
{
	m_entities.insert(m_entities.end(), entities.begin(), entities.end());

	for (std::vector<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		Entity* pEnt = *it;
		entityAdded(pEnt);
	}
}

// @UNUSED
void Level::ensureAdded(Entity* entity)
{
	int chunkX = Mth::floor(entity->m_pos.x / 16.0f);
	int chunkZ = Mth::floor(entity->m_pos.z / 16.0f);

	// force the chunk to be resolved
	for (int x = chunkX - 2; x <= chunkX + 2; x++)
		for (int z = chunkZ - 2; z <= chunkZ + 2; z++)
			getChunk(x, z);

	EntityVector::iterator result = std::find(m_entities.begin(), m_entities.end(), entity);
	if (result == m_entities.end())
		m_entities.push_back(entity);
}

void Level::extinguishFire(int x, int y, int z, int dir)
{
	switch (dir)
	{
		case DIR_YNEG: y--; break;
		case DIR_YPOS: y++; break;
		case DIR_ZNEG: z--; break;
		case DIR_ZPOS: z++; break;
		case DIR_XNEG: x--; break;
		case DIR_XPOS: x++; break;
	}

	if (getTile(x, y, z) == Tile::fire->m_ID)
		setTile(x, y, z, TILE_AIR);
}

int Level::findPath(Path* path, Entity* ent, Entity* target, float f)
{
	int fx = Mth::floor(ent->m_pos.x);
	int fy = Mth::floor(ent->m_pos.y);
	int fz = Mth::floor(ent->m_pos.z);
	Region reg(this, fx - int(f + 16), fy - int(f + 16), fz - int(f + 16), fx + int(f + 16), fy + int(f + 16), fz + int(f + 16));

	m_pPathFinder->setLevel(&reg);
	m_pPathFinder->findPath(*path, ent, target->m_pos.x, target->m_hitbox.min.y, target->m_pos.z, f);

	return 1;
}

int Level::findPath(Path* path, Entity* ent, int dx, int dy, int dz, float f)
{
	int fx = Mth::floor(ent->m_pos.x);
	int fy = Mth::floor(ent->m_pos.y);
	int fz = Mth::floor(ent->m_pos.z);
	Region reg(this, fx - int(f + 8), fy - int(f + 8), fz - int(f + 8), fx + int(f + 8), fy + int(f + 8), fz + int(f + 8));
	
	m_pPathFinder->setLevel(&reg);
	m_pPathFinder->findPath(*path, ent, float(dx) + 0.5f, float(dy) + 0.5f, float(dz) + 0.5f, f);

	return 1;
}

int Level::getLightDepth(int x, int z)
{
	return getChunk(x, z)->getHeightmap(x & 0xF, z & 0xF);
}

float Level::getStarBrightness(float f)
{
	float ca = Mth::cos(getSunAngle(f));
	float cb = 1.0f - (0.75f + 2 * ca);

	if (cb < 0.0f)
		cb = 0.0f;
	if (cb > 1.0f)
		cb = 1.0f;

	return cb * cb * 0.5f;
}

float Level::getSunAngle(float f)
{
	return 2 * float(M_PI) * getTimeOfDay(f);
}
