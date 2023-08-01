/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"
#include "ChunkCache.hpp"
#include "Util.hpp"
#include "Explosion.hpp"
#include "Region.hpp"

Level::Level(LevelStorage* pStor, const std::string& str, TLong seed, int x) :
	field_38(1), // initialize with a seed of 1
	m_pLevelStorage(pStor)
{
	_init(str, seed, x, nullptr);
}

Level::Level(LevelStorage* pStor, const std::string& str, TLong seed, int x, Dimension *pDimension) :
	field_38(1), // initialize with a seed of 1
	m_pLevelStorage(pStor)
{
	_init(str, seed, x, pDimension);
}

Level::~Level()
{
	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pDimension);

	for (auto pEnt : m_entities)
		delete pEnt;
	
	m_entities.clear();
}

void Level::_init(const std::string& str, TLong  seed, int x, Dimension* pDimension)
{
	field_12 = 0;

	LevelData* pData = m_pLevelStorage->prepareLevel(this);

	field_B0C = pData == 0;

	if (pDimension)
		m_pDimension = pDimension;
	else
		m_pDimension = new Dimension;

	if (!pData)
		m_levelData = LevelData(seed, str, x);
	else
		m_levelData = *pData;

	m_pDimension->init(this);

	m_pChunkSource = createChunkSource();
	updateSkyBrightness();
}

ChunkSource* Level::createChunkSource()
{
#ifndef MOD_USE_FLAT_WORLD
	if (m_pLevelStorage)
	{
		ChunkStorage* pChkStr = m_pLevelStorage->createChunkStorage(m_pDimension);
		ChunkSource* pChkSrc = m_pDimension->createRandomLevelSource();
		return new ChunkCache(this, pChkStr, pChkSrc);
	}
#endif

	puts("no level data, calling dimension->createRandomLevelSource");
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
	for (auto pEnt : m_entities)
	{
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

	for (auto pListener : m_levelListeners)
		pListener->tileBrightnessChanged(x, y, z);
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

	if (m_pDimension->field_E && &ll == &LightLayer::Sky || !m_bUpdateLights)
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

	if (!unimportant || m_lightUpdates.size() <= 0)
	{
	label_20:
		m_lightUpdates.push_back(LightUpdate(ll, a, b, c, d, e, f));
		nUpdateLevels--;
		return;
	}

	for (int i = 0; i < int(m_lightUpdates.size()); i++)
	{
		//dont know what the hell this is v19 = &v15[~v17 - 0x49249249 * (i >> 2)];

		LightUpdate* pUpd = &m_lightUpdates[i];
		if (pUpd->m_lightLayer == &ll && pUpd->expandToContain(a, b, c, d, e, f))
		{
			nUpdateLevels--;
			return;
		}
	}
	goto label_20;
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
	for (auto pListener : m_levelListeners)
		pListener->tileChanged(x, y, z);
}

void Level::neighborChanged(int x, int y, int z, TileID tile)
{
	if (field_30 || field_11) return;

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
	for (auto pListener : m_levelListeners)
		pListener->setTilesDirty(x1, y1, z1, x2, y2, z2);
}

void Level::entityAdded(Entity* pEnt)
{
	for (auto pListener : m_levelListeners)
		pListener->entityAdded(pEnt);
}

void Level::entityRemoved(Entity* pEnt)
{
	for (auto pListener : m_levelListeners)
		pListener->entityRemoved(pEnt);
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

	for (Player* player : m_players)
	{
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
			spawnX += (field_38.genrand_int32() % 8) - (field_38.genrand_int32() % 8);
			spawnZ += (field_38.genrand_int32() % 8) - (field_38.genrand_int32() % 8);

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

	printf("Failed to validate spawn point, using (%d, %d, %d)\n", m_levelData.m_spawnPos.x, m_levelData.m_spawnPos.y, m_levelData.m_spawnPos.z);

	return;
#endif
}

void Level::removeAllPendingEntityRemovals()
{
	Util::removeAll(m_entities, m_pendingEntityRemovals);

	for (auto ent : m_pendingEntityRemovals)
	{
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
		removeEntity(pOldEnt);

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

void Level::setInitialSpawn()
{
	field_A00 = true;

	int spawnX = C_MAX_CHUNKS_X * 16 / 2, spawnZ = C_MAX_CHUNKS_Z * 16 / 2;
#ifndef ORIGINAL_CODE
	int nAttempts = 0;
#endif

	while (true)
	{
		if (m_pDimension->isValidSpawn(spawnX, spawnZ))
			break;

		spawnX += (field_38.genrand_int32() % 32) - (field_38.genrand_int32() % 32);
		spawnZ += (field_38.genrand_int32() % 32) - (field_38.genrand_int32() % 32);

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

	field_A00 = false;

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

	printf("Failed to validate spawn point, using (%d, %d, %d)\n", m_levelData.m_spawnPos.x, m_levelData.m_spawnPos.y, m_levelData.m_spawnPos.z);

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

bool Level::isUnobstructed(AABB* aabb)
{
	EntityVector* entities = getEntities(nullptr, *aabb);
	if (entities->size() <= 0)
		return true;

	for (auto pEnt : *entities)
	{
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
	auto iter = std::find(m_levelListeners.begin(), m_levelListeners.end(), listener);
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
				Tile::tiles[tile]->tick(this, t.field_4, t.field_8, t.field_C, &field_38);
		}

		m_pendingTicks.erase(m_pendingTicks.begin());
	}
}

void Level::tickTiles()
{
	m_chunksToUpdate.clear();

	for (auto player : m_players)
	{
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

	for (auto pos : m_chunksToUpdate)
	{
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
				Tile::tiles[tile]->tick(this, tileX + globX, tileY, tileZ + globZ, &field_38);
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
#ifndef ORIGINAL_CODE
			LogMsg("Not updating entity %d because we don't have chunks around it loaded", pEnt->m_EntityID);
#endif
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

		for (auto pListener : m_levelListeners)
			pListener->skyColorChanged();
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

		//@NOTE: Here, they actually delete the entity. In the entity removals thing they don't
		delete pEnt;
	}
}
/*
HitResult Level::clip(Vec3 vec1, Vec3 vec2, bool flag)
{
	// @NOTE: Using a version from Minecraft Java Edition Beta 1.6, decompiled courtesy of the Mod Coder Pack.
	// The name there is "rayTraceBlocks", but I'm convinced that it's also called "Level.clip" in Notch's original code.
	// They look pretty alike though, so I believe that this is going to work

	int v2x = Mth::floor(vec2.x);
	int v2y = Mth::floor(vec2.y);
	int v2z = Mth::floor(vec2.z);
	int v1x = Mth::floor(vec1.x);
	int v1y = Mth::floor(vec1.y);
	int v1z = Mth::floor(vec1.z);

	TileID tile = getTile(v1x, v1y, v1z);
	int    data = getData(v1x, v1y, v1z);
	Tile *pTile = Tile::tiles[tile];
	/ *
	if (//(!flag || !pTile || !pTile->getAABB(this, v1x, v1y, v1z)) && 
		tile > 0 && pTile->mayPick(data, flag))
	{
		HitResult hr = pTile->clip(this, v1x, v1y, v1z, vec1, vec2);
		if (hr.m_hitType != HitResult::NONE)
			return hr;
	}
	* /

	for (int i = 200; i >= 0; i--)
	{
		if (v1x == v2x && v1y == v2y && v1z == v2z)
			return HitResult();

		float f0 = 999.0f, f1 = 999.0f, f2 = 999.0f;
		bool flag2 = true, flag3 = true, flag4 = true;

		if (v2x > v1x)
			f0 = float(v1x) + 1.0f;
		else if (v2x < v1x)
			f0 = float(v1x) + 0.0f;
		else
			flag2 = false;

		if (v2y > v1y)
			f1 = float(v1y) + 1.0f;
		else if (v2y < v1y)
			f1 = float(v1y) + 0.0f;
		else
			flag3 = false;

		if (v2z > v1z)
			f2 = float(v1z) + 1.0f;
		else if (v2z < v1z)
			f2 = float(v1z) + 0.0f;
		else
			flag4 = false;

		float f3 = 999.0f, f4 = 999.0f, f5 = 999.0f;
		float f6 = vec2.x - vec1.z, f7 = vec2.y - vec1.y, f8 = vec2.z - vec1.z;

		if (flag2) f3 = (f0 - vec1.x) / f6;
		if (flag3) f4 = (f1 - vec1.y) / f7;
		if (flag4) f5 = (f2 - vec1.z) / f8;

		HitResult::eHitSide hitSide = HitResult::MINY;
		if (f3 < f4 && f3 < f5)
		{
			if (v2x > v1x) hitSide = HitResult::MINX;
			else           hitSide = HitResult::MAXX;
			vec1.x = f0;
			vec1.y += f7 * f3;
			vec1.z += f8 * f3;
		}
		else if (f4 < f5)
		{
			if (v2y > v1y) hitSide = HitResult::MINY;
			else           hitSide = HitResult::MAXY;
			vec1.x += f6 * f4;
			vec1.y = f1;
			vec1.z += f8 * f4;
		}
		else
		{
			if (v2z > v1z) hitSide = HitResult::MINZ;
			else           hitSide = HitResult::MAXZ;
			vec1.x += f6 * f5;
			vec1.y += f7 * f5;
			vec1.z = f2;
		}

		Vec3 vec = vec1;

		v1x = int(vec.x = float(Mth::floor(vec1.x)));
		if (hitSide == HitResult::MAXX)
		{
			v1x--;
			vec.x -= 1.0f;
		}

		v1y = int(vec.y = float(Mth::floor(vec1.y)));
		if (hitSide == HitResult::MAXY)
		{
			v1y--;
			vec.y -= 1.0f;
		}

		v1z = int(vec.z = float(Mth::floor(vec1.z)));
		if (hitSide == HitResult::MAXZ)
		{
			v1z--;
			vec.z -= 1.0f;
		}

		TileID tile = getTile(v1x, v1y, v1z);
		int    data = getData(v1x, v1y, v1z);
		Tile* pTile = Tile::tiles[tile];

		if (//(!flag || !pTile || !pTile->getAABB(this, v1x, v1y, v1z)) && 
			tile > 0 && pTile->mayPick(data, flag))
		{
			HitResult hr = pTile->clip(this, v1x, v1y, v1z, vec1, vec2);

			if (hr.m_hitType != HitResult::NONE)
				return hr;
		}
	}

	return HitResult();
}
*/

HitResult Level::clip(Vec3 vecA, Vec3 vecB, bool b)
{
	float v5; // s14
	int v6; // s15
	float v7; // s18
	float v10; // s16
	bool v11; // r8
	float v12; // s17
	float v13; // s19
	float v14; // s20
	float v15; // s21
	int v17; // r11
	int v18; // r4
	int v19; // r5
	int v20; // r10
	bool v21; // r3
	float v22; // s10
	float v23; // s11
	float v24; // s12
	float v25; // s9
	float v26; // s15
	float v27; // s8
	float v28; // s7
	float v29; // s13
	int v30; // r7
	TileID v31; // r9
	int v32; // r1
	Tile* v33; // r8
	Vec3 v50; // [sp+8h] [bp-C8h] BYREF
	Vec3 v51; // [sp+14h] [bp-BCh] BYREF
	int v52; // [sp+24h] [bp-ACh]
	int v53; // [sp+28h] [bp-A8h]
	int v54; // [sp+2Ch] [bp-A4h]
	Vec3* v56; // [sp+3Ch] [bp-94h]
	Vec3* v57; // [sp+40h] [bp-90h]
	bool v58; // [sp+44h] [bp-8Ch]
	HitResult result; // [sp+50h] [bp-80h] BYREF

	v7 = vecA.z;
	v10 = vecA.x;
	v11 = b;
	v12 = vecA.y;
	v13 = vecB.x;
	v14 = vecB.y;
	v15 = vecB.z;
	v54 = Mth::floor(vecB.x);
	v52 = Mth::floor(v14);
	v53 = Mth::floor(v15);
	v17 = Mth::floor(vecA.x);
	v18 = Mth::floor(vecA.y);
	v56 = &v51;
	v57 = &vecB;
	v58 = v11;
	v19 = Mth::floor(vecA.z);
	v20 = 199;
	while (1)
	{
		if (v18 == v52)
			v21 = v17 == v54;
		else
			v21 = 0;
		if (v21)
		{
			if (v19 == v53)
				goto LABEL_43;
		LABEL_8:
			v22 = 999.0f;
			goto LABEL_9;
		}
		if (v54 <= v17)
		{
			if (v54 >= v17)
				goto LABEL_8;
			v6 = v17;
			v22 = float(v17) + 0.0f;
		}
		else
		{
			v6 = v17;
			v22 = float(v17) + 1.0f;
		}
	LABEL_9:
		if (v52 <= v18)
		{
			if (v52 >= v18)
			{
				v23 = 999.0f;
			}
			else
			{
				v6 = v18;
				v23 = float(v18);
			}
			if (v52 < v18)
				v23 = v23 + 0.0f;
		}
		else
		{
			v6 = v18;
			v23 = float(v18) + 1.0f;
		}
		if (v53 <= v19)
		{
			if (v53 >= v19)
			{
				v24 = 999.0f;
			}
			else
			{
				v6 = v19;
				v24 = float(v19);
			}
			if (v53 < v19)
				v24 = v24 + 0.0f;
		}
		else
		{
			v6 = v19;
			v24 = float(v19) + 1.0f;
		}

		float v6_2;

		v25 = v13 - v10;
		if (v22 == 999.0f)
			v5 = 999.0f;
		else
			v6_2 = v22 - v10;
		if (v22 != 999.0f)
			v5 = v6_2 / v25;
		if (v23 == 999.0f)
			v26 = 999.0f;
		else
			v26 = v23 - v12;
		v27 = v14 - v12;
		if (v23 != 999.0f)
			v26 = v26 / v27;
		v28 = v15 - v7;
		if (v24 == 999.0f)
			v29 = 999.0f;
		else
			v29 = v24 - v7;
		if (v24 != 999.0f)
			v29 = v29 / v28;
		if (v5 >= v26 || v5 >= v29)
		{
			if (v26 >= v29)
			{
				v10 = v10 + float(v25 * v29);
				if (v53 <= v19)
					v30 = 3;
				else
					v30 = 2;
				v12 = v12 + float(v27 * v29);
				v7 = v24;
			}
			else
			{
				v10 = v10 + float(v25 * v26);
				v30 = v18 >= v52;
				v7 = v7 + float(v28 * v26);
				v12 = v23;
			}
		}
		else
		{
			v12 = v12 + float(v27 * v5);
			if (v54 <= v17)
				v30 = 5;
			else
				v30 = 4;
			v7 = v7 + float(v28 * v5);
			v10 = v22;
		}
		v17 = Mth::floor(v10);
		if (v30 == 5)
		{
			--v17;
			v18 = Mth::floor(v12);
			goto LABEL_65;
		}
		v18 = Mth::floor(v12);
		if (v30 == 1)
		{
			--v18;
		LABEL_65:
			v6 = Mth::floor(v7);
			v19 = v6;
			goto LABEL_38;
		}
		v6 = Mth::floor(v7);
		v19 = v6;
		if (v30 == 3)
			v19 = v6 - 1;
	LABEL_38:
		v31 = getTile(v17, v18, v19);
		v32 = getData(v17, v18, v19);
		v33 = Tile::tiles[v31];
		if (v31 > 0)
		{
			if (v33->mayPick(v32, v58))
			{
				//v34 = v33->m_vtable->clip;
				vecA.x = v10;
				vecA.y = v12;
				vecA.z = v7;
				result = v33->clip(this, v17, v18, v19, vecA, *v57);
				if (result.m_hitType != HitResult::NONE)
					break;
			}
		}
		if (v20 == -1)
		{
		LABEL_43:
			result.m_hitPos = Vec3();
			result.m_hitType = HitResult::NONE;
			return result;
		}
		--v20;
	}
	
	return result;
}

HitResult Level::clip(const Vec3& a, const Vec3& b)
{
	return clip(a, b, false);
}

void Level::addToTickNextTick(int a, int b, int c, int d, int delay)
{
	TickNextTickData tntd(a, b, c, d);
	if (field_10)
	{
		// @NOTE: Don't know why this check wasn't just placed at the beginning.
		if (!hasChunksAt(a - 8, b - 8, c - 8, a + 8, b + 8, c + 8))
			return;

		TileID tile = getTile(tntd.field_4, tntd.field_8, tntd.field_C);
		if (tile > 0 && tile == tntd.field_10)
			Tile::tiles[tntd.field_10]->tick(this, tntd.field_4, tntd.field_8, tntd.field_C, &field_38);
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
	for (auto pListener : m_levelListeners)
		pListener->takePicture(pCamera, pOwner);
}

void Level::addParticle(const std::string& name, float a, float b, float c, float d, float e, float f)
{
	for (auto pListener : m_levelListeners)
		pListener->addParticle(name, a, b, c, d, e, f);
}

void Level::playSound(Entity* entity, const std::string& name, float a, float b)
{
	for (auto pListener : m_levelListeners)
		pListener->playSound(name, entity->m_pos.x, entity->m_pos.y - entity->field_84, entity->m_pos.z, a, b);
}

void Level::playSound(float x, float y, float z, const std::string& name, float a, float b)
{
	for (auto pListener : m_levelListeners)
		pListener->playSound(name, x, y, z, a, b);
}

void Level::animateTick(int x, int y, int z)
{
	Random random;

	for (int i = 0; i < 100; i++)
	{
		int x1, y1, z1;
		x1 = x + (field_38.genrand_int32() & 0xF) - (field_38.genrand_int32() & 0xF);
		y1 = y + (field_38.genrand_int32() & 0xF) - (field_38.genrand_int32() & 0xF);
		z1 = z + (field_38.genrand_int32() & 0xF) - (field_38.genrand_int32() & 0xF);
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

	for (Entity* pEnt : entities)
		entityAdded(pEnt);
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

int Level::findPath(Entity* ent1, Entity* ent2, float f)
{
	int fx = Mth::floor(ent1->m_pos.x);
	int fy = Mth::floor(ent1->m_pos.y);
	int fz = Mth::floor(ent1->m_pos.z);
	Region reg(this, fx - int(f + 16), fy - int(f + 16), fz - int(f + 16), fx + int(f + 16), fy + int(f + 16), fz + int(f + 16));
	// nothing more.
	return 0;
}

int Level::findPath(Entity* ent1, int dx, int dy, int dz, float f)
{
	int fx = Mth::floor(ent1->m_pos.x);
	int fy = Mth::floor(ent1->m_pos.y);
	int fz = Mth::floor(ent1->m_pos.z);
	Region reg(this, fx - int(f + 16), fy - int(f + 16), fz - int(f + 16), fx + int(f + 16), fy + int(f + 16), fz + int(f + 16));
	// nothing more.
	return 0;
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
