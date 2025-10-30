/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"

#include <algorithm>

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "network/RakNetInstance.hpp"
#include "network/packets/EntityEventPacket.hpp"
#include "network/packets/SetEntityDataPacket.hpp"
#include "world/level/levelgen/chunk/ChunkCache.hpp"

#include "Explosion.hpp"
#include "Region.hpp"

Level::Level(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion, Dimension *pDimension)
{
	m_bInstantTicking = false;
	m_bIsClientSide = false;
	m_bPostProcessing = false;
	m_skyDarken = 0;
	field_30 = 0;
	m_pDimension = nullptr;
    m_difficulty = 2; // Java has no actual default, it just always pulls from Options. Putting 2 here just so there's no chance of mobs getting despawned accidentally.
	m_pRakNetInstance = nullptr;
	m_bCalculatingInitialSpawn = false;
	m_pChunkSource = nullptr;
	m_pLevelStorage = pStor;
	m_randValue = 42184323;
	m_addend = 1013904223;
	m_bUpdateLights = true;
	field_B08 = 0;
	field_B0C = 0;

	m_random.setSeed(1); // initialize with a seed of 1

	LevelData* pData = m_pLevelStorage->prepareLevel(this);

	field_B0C = pData == nullptr;

	// @BUG: leaking a Dimension*?
	if (pDimension)
		m_pDimension = pDimension;
	else
		m_pDimension = new Dimension;

	if (!pData)
		m_pLevelData = new LevelData(settings, name, storageVersion);
	else
		m_pLevelData = pData;

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
		
		//you better HOPE this is freed by Minecraft! (or a NetworkHandler)
		//Really should have used shared pointers and stuff.
		if (!pEnt->isPlayer())
			delete pEnt;
	}
	
	m_entities.clear();
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

	LOG_I("No level data, calling dimension->createRandomLevelSource");
	return m_pDimension->createRandomLevelSource();
}

float Level::getTimeOfDay(float f)  const
{
	return m_pDimension->getTimeOfDay(getTime(), f);
}

int Level::getSkyDarken(float f) const
{
	float x = Mth::cos(getSunAngle(f));
	float y = 1.0f - (2 * x + 0.5f);

	if (y < 0.0f)
		return 0; // no darken
	// 0.1.0 logic
	/*if (y > 1.0f)
		return 11; // full dark*/
	// 0.2.1 logic
	if (y > 0.8f)
		return 8; // full dark

	return int(y * 11.0f);
}

void Level::updateSkyDarken()
{
	bool skyColorChanged = updateSkyBrightness();

	if (skyColorChanged)
	{
		for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
		{
			LevelListener* pListener = *it;
			pListener->skyColorChanged();
		}
	}
}

bool Level::updateSkyBrightness()
{
	int skyDarken = getSkyDarken(1.0f);
	if (m_skyDarken != skyDarken)
	{
		m_skyDarken = skyDarken;
		return true;
	}

	return false;
}

BiomeSource* Level::getBiomeSource() const
{
	return m_pDimension->m_pBiomeSource;
}

ChunkSource* Level::getChunkSource() const
{
	return m_pChunkSource;
}

LevelChunk* Level::getChunk(const ChunkPos& pos) const
{
	return getChunkSource()->getChunk(pos);
}

TileID Level::getTile(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getTile(pos);
}

TileData Level::getData(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getData(pos);
}

int Level::getBrightness(const LightLayer& ll, const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return ll.m_x;

	if (!hasChunk(pos))
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getBrightness(ll, pos);
}

float Level::getBrightness(const TilePos& pos) const
{
	return m_pDimension->field_10[getRawBrightness(pos)];
}

int Level::getRawBrightness(const TilePos& pos) const
{
	return getRawBrightness(pos, true);
}

int Level::getRawBrightness(const TilePos& pos, bool b) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z)
		return 15;

	// this looks like some kind of hack.
	if (b && (getTile(pos) == Tile::stoneSlabHalf->m_ID || getTile(pos) == Tile::farmland->m_ID))
	{
		int b1 = getRawBrightness(pos.above(), false);
		int b2 = getRawBrightness(pos.east(), false);
		int b3 = getRawBrightness(pos.west(), false);
		int b4 = getRawBrightness(pos.south(), false);
		int b5 = getRawBrightness(pos.north(), false);

		if (b2 < b1) b2 = b1;
		if (b3 < b2) b3 = b2;
		if (b4 < b3) b4 = b3;
		if (b5 < b4) b5 = b4;

		return b5;
	}

	if (pos.y < C_MIN_Y)
		return 0;

	if (pos.y >= C_MAX_Y)
	{
		int r = 15 - m_skyDarken;
		if (r < 0)
			r = 0;

		return r;
	}

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getRawBrightness(pos, m_skyDarken);
}

void Level::swap(const TilePos& pos1, const TilePos& pos2)
{
	TileID tile1 = getTile(pos1);
	TileData data1 = getData(pos1);
	TileID tile2 = getTile(pos2);
	TileData data2 = getData(pos2);

	setTileAndDataNoUpdate(pos1, tile2, data2);
	setTileAndDataNoUpdate(pos2, tile1, data1);

	updateNeighborsAt(pos1, tile2);
	updateNeighborsAt(pos2, tile1);
}

bool Level::isDay() const
{
	return m_skyDarken <= 3;
}

bool Level::isEmptyTile(const TilePos& pos) const
{
	return getTile(pos) == 0;
}

bool Level::isSolidTile(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];
	if (!pTile) return false;

	return pTile->isSolidRender();
}

Material* Level::getMaterial(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];
	if (!pTile) return Material::air;

	return pTile->m_pMaterial;
}

Entity* Level::getEntity(Entity::ID id) const
{
	// @TODO: wtf? no map??

	// prioritize players first.
	for (std::vector<Player*>::const_iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Player* pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}
	for (std::vector<Entity*>::const_iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		Entity* pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}

	return nullptr;
}

const EntityVector* Level::getAllEntities() const
{
	return &m_entities;
}

bool Level::hasChunk(const ChunkPos& pos) const
{
	return m_pChunkSource->hasChunk(pos);
}

EntityVector Level::getEntities(Entity* pEntExclude, const AABB& aabb) const
{
	EntityVector entities = EntityVector();

	long lowerXBound = floor((aabb.min.x - 2.0f) / 16);
	long lowerZBound = floor((aabb.min.z - 2.0f) / 16);
	long upperXBound = floor((aabb.max.x + 2.0f) / 16);
	long upperZBound = floor((aabb.max.z + 2.0f) / 16);

	for (long z = lowerZBound; z <= upperZBound; z++)
	{
		for (long x = lowerXBound; x <= upperXBound; x++)
		{
			if (!hasChunk(ChunkPos(x, z))) continue;

			LevelChunk* pChunk = getChunk(ChunkPos(x, z));
			pChunk->getEntities(pEntExclude, aabb, entities);
		}
	}

	return entities;
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

bool Level::hasChunksAt(const TilePos& min, const TilePos& max) const
{
	if (min.y >= C_MAX_Y || max.y < 0)
		return false;

	ChunkPos cpMin(min), cpMax(max), cp = ChunkPos();
	for (cp.x = cpMin.x; cp.x <= cpMax.x; cp.x++)
	{
		for (cp.z = cpMin.z; cp.z <= cpMax.z; cp.z++)
		{
			if (!hasChunk(cp))
				return false;
		}
	}

	return true;
}

bool Level::hasChunksAt(const TilePos& pos, int rad) const
{
	return hasChunksAt(pos - rad, pos + rad);
}

void Level::setBrightness(const LightLayer& ll, const TilePos& pos, int brightness)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return;

	LevelChunk* pChunk = getChunk(pos);
	pChunk->setBrightness(ll, pos, brightness);

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileBrightnessChanged(pos);
	}
}

void Level::setTime(int32_t time)
{
	_setTime(time);

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->timeChanged(time);
	}
}

int Level::getDirectSignal(const TilePos& pos, Facing::Name face) const
{
	TileID tile = getTile(pos);
	if (!tile) return 0;

	return Tile::tiles[tile]->getDirectSignal(this, pos, face);
}

int Level::getSignal(const TilePos& pos, Facing::Name face) const
{
	if (isSolidTile(pos))
		return hasDirectSignal(pos);

	TileID tile = getTile(pos);
	if (!tile) return 0;

	return Tile::tiles[tile]->getSignal(this, pos, face);
}

bool Level::hasDirectSignal(const TilePos& pos) const
{
	if (getDirectSignal(pos.below(), Facing::DOWN)) return true;
	if (getDirectSignal(pos.above(), Facing::UP)) return true;
	if (getDirectSignal(pos.north(), Facing::NORTH)) return true;
	if (getDirectSignal(pos.south(), Facing::SOUTH)) return true;
	if (getDirectSignal(pos.west(), Facing::WEST)) return true;
	if (getDirectSignal(pos.east(), Facing::EAST)) return true;
	return false;
}

bool Level::hasNeighborSignal(const TilePos& pos) const
{
	if (getSignal(pos.below(), Facing::DOWN)) return true;
	if (getSignal(pos.above(), Facing::UP)) return true;
	if (getSignal(pos.north(), Facing::NORTH)) return true;
	if (getSignal(pos.south(), Facing::SOUTH)) return true;
	if (getSignal(pos.west(), Facing::WEST)) return true;
	if (getSignal(pos.east(), Facing::EAST)) return true;
	return false;
}

bool Level::hasChunkAt(const TilePos& pos) const
{
	return hasChunk(pos);
}

LevelChunk* Level::getChunkAt(const TilePos& pos) const
{
	return getChunk(pos);
}

void Level::updateLight(const LightLayer& ll, const TilePos& tilePos1, const TilePos& tilePos2, bool unimportant)
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

	TilePos idkbro((tilePos2.x + tilePos1.x) / 2, (tilePos2.y + tilePos1.y) / 2, (tilePos2.z + tilePos1.z) / 2);

	if (!hasChunkAt(idkbro) || getChunkAt(idkbro)->isEmpty())
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
			if (update.m_lightLayer == &ll && update.expandToContain(tilePos1, tilePos2))
			{
				nUpdateLevels--;
				return;
			}
		}
	}

	m_lightUpdates.push_back(LightUpdate(ll, tilePos1, tilePos2));

	// huh??
	if (m_lightUpdates.size() > 1000000)
		m_lightUpdates.clear();

	nUpdateLevels--;
}

void Level::updateLight(const LightLayer& ll, const TilePos& tilePos1, const TilePos& tilePos2)
{
	updateLight(ll, tilePos1, tilePos2, true);
}

void Level::updateLightIfOtherThan(const LightLayer& ll, const TilePos& tilePos, int bright)
{
	if (m_pDimension->field_E && &ll == &LightLayer::Sky)
		return;

	if (!hasChunkAt(tilePos))
		return;

	if (&ll == &LightLayer::Sky)
	{
		if (isSkyLit(tilePos))
			bright = 15;
	}
	else if (&ll == &LightLayer::Block)
	{
		TileID tile = getTile(tilePos);
		if (bright < Tile::lightEmission[tile])
			bright = Tile::lightEmission[tile];
	}

	int oldbr = getBrightness(ll, tilePos);
	if (bright != oldbr)
	{
		updateLight(ll, tilePos, tilePos);
	}
}

bool Level::isSkyLit(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y)
		// there's nothing out there!
		return false;

	if (pos.y >= C_MAX_Y)
		return true;

	if (!hasChunk(pos))
		return false;

	return getChunk(pos)->isSkyLit(pos);
}

bool Level::setTileAndDataNoUpdate(const TilePos& pos, TileID tile, TileData data)
{
	return setTileAndData(pos, tile, data, TileChange::UPDATE_SILENT);
}

int Level::getHeightmap(const TilePos& pos)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z)
		// there's nothing out there!
		return 0;

	if (!hasChunk(pos))
		return 0;

	return getChunkAt(pos)->getHeightmap(pos);
}

void Level::lightColumnChanged(int x, int z, int y1, int y2)
{
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	setTilesDirty(TilePos(x, y1, z), TilePos(x, y2, z));
}

bool Level::setDataNoUpdate(const TilePos& pos, TileData data)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(pos))
		return false;

	LevelChunk* pChk = getChunk(pos);
	if (pChk->getData(pos) == data)
		return false; // no update

	pChk->setData(pos, data);
	return true;
}

bool Level::setTileNoUpdate(const TilePos& pos, TileID tile)
{
	return setTileAndDataNoUpdate(pos, tile, 0);
}

void Level::sendTileUpdated(const TilePos& pos)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileChanged(pos);
	}
}

void Level::neighborChanged(const TilePos& pos, TileID tile)
{
	if (field_30 || m_bIsClientSide) return;

	Tile* pTile = Tile::tiles[getTile(pos)];
	if (pTile)
		pTile->neighborChanged(this, pos, tile);
}

void Level::updateNeighborsAt(const TilePos& pos, TileID tile)
{
	neighborChanged(pos.west(), tile);
	neighborChanged(pos.east(), tile);
	neighborChanged(pos.below(), tile);
	neighborChanged(pos.above(), tile);
	neighborChanged(pos.north(), tile);
	neighborChanged(pos.south(), tile);
}

void Level::tileUpdated(const TilePos& pos, TileID tile)
{
	//sendTileUpdated(pos); // not in 0.7.0
	updateNeighborsAt(pos, tile);
}

bool Level::setTileAndData(const TilePos& pos, TileID tile, TileData data, TileChange::UpdateFlags updateFlags)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(pos))
		return false;

	LevelChunk* pChunk = getChunk(pos);

	TileChange change(updateFlags);

	TileID oldTile = TILE_AIR;
	if (change.isUpdateNeighbors())
		oldTile = pChunk->getTile(pos);

	bool result = pChunk->setTileAndData(pos, tile, data);
	if (result)
	{
		if (change.isUpdateListeners() && (!m_bIsClientSide || !change.isUpdateSilent()))
		{
			// Send update to level listeners
			sendTileUpdated(pos);
		}
		if (!m_bIsClientSide && change.isUpdateNeighbors())
		{
			// Update neighbors
			tileUpdated(pos, oldTile);
		}
	}

	return result;

	/*if (setTileAndDataNoUpdate(pos, tile, data))
	{
		tileUpdated(pos, tile);
		return true;
	}
	return false;*/
}

bool Level::setData(const TilePos& pos, TileData data, TileChange::UpdateFlags updateFlags)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	LevelChunk* pChunk = getChunk(pos);
	if (!pChunk)
		return false;

	TileChange change(updateFlags);

	bool result = pChunk->setData(pos, data);
	if (result)
	{
		TileID tileId = pChunk->getTile(pos);

		if (change.isUpdateListeners() && (!m_bIsClientSide || !change.isUpdateSilent()))
		{
			// Send update to level listeners
			sendTileUpdated(pos);
		}
		if (!m_bIsClientSide && change.isUpdateNeighbors())
		{
			// Update neighbors
			tileUpdated(pos, tileId);
		}
	}

	return result;

	/*if (setDataNoUpdate(pos, data))
	{
		tileUpdated(pos, getTile(pos));
		return true;
	}
	return false;*/
}

bool Level::setTile(const TilePos& pos, TileID tile, TileChange::UpdateFlags updateFlags)
{
	return setTileAndData(pos, tile, 0, updateFlags);
}

void Level::setTilesDirty(const TilePos& min, const TilePos& max)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->setTilesDirty(min, max);
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

void Level::levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->levelEvent(pPlayer, eventId, pos, data);
	}
}

AABBVector* Level::getCubes(const Entity* pEntUnused, const AABB& aabb)
{
	m_aabbs.clear();

	long lowerX = floor(aabb.min.x);
	long upperX = floor(aabb.max.x + 1);
	long lowerY = floor(aabb.min.y);
	long upperY = floor(aabb.max.y + 1);
	long lowerZ = floor(aabb.min.z);
	long upperZ = floor(aabb.max.z + 1);

	for (long x = lowerX; x <= upperX; x++)
	{
		for (long z = lowerZ; z <= upperZ; z++)
		{
			if (!hasChunkAt(TilePos(x, 64, z))) continue;

			// - 1 fixes tiles like the fence
			for (long y = lowerY - 1; y <= upperY; y++)
			{
				// Obviously this is problematic, but using longs in our for loops rather than
				// ints helps prevents crashes at extreme distances from 0,0
				TilePos tp((int)x, (int)y, (int)z);
				Tile* pTile = Tile::tiles[getTile(tp)];
				if (pTile)
					pTile->addAABBs(this, tp, &aabb, m_aabbs);
			}
		}
	}

	return &m_aabbs;
}

std::vector<LightUpdate>* Level::getLightsToUpdate()
{
	return &m_lightUpdates;
}

Player* Level::_getNearestPlayer(const Vec3& source, float maxDist, bool onlyFindAttackable) const
{
	float dist = -1.0f;
	Player* pPlayer = nullptr;

	for (std::vector<Player*>::const_iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Player* player = *it;

		if (onlyFindAttackable)
		{
			if (player->isCreative() || !player->isAlive())
				continue;
		}

		float ldist = player->distanceToSqr(source);
		if ((maxDist < 0.0f || ldist < maxDist * maxDist) && (dist == -1.0f || dist > ldist))
		{
			pPlayer = player;
			dist = ldist;
		}
	}

	return pPlayer;
}

Player* Level::getNearestPlayer(const Entity& source, float maxDist) const
{
	return getNearestPlayer(source.m_pos, maxDist, false);
}

Player* Level::getNearestPlayer(const Vec3& source, float maxDist, bool findAnyNearPlayer = false) const
{
	return _getNearestPlayer(source, maxDist, false); // don't know what findAnyNearPlayer is actually supposed to do
}

Player* Level::getNearestAttackablePlayer(const Entity& source, float maxDist) const
{
	return getNearestAttackablePlayer(source.m_pos, maxDist, &source);
}

Player* Level::getNearestAttackablePlayer(const Vec3& source, float maxDist, const Entity* sourceEntity = nullptr) const
{
	return _getNearestPlayer(source, maxDist, true);
}

bool Level::containsFireTile(const AABB& aabb)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
				if (tileID == Tile::fire->m_ID || tileID == Tile::lava->m_ID || tileID == Tile::calmLava->m_ID)
					return true;
			}

	return false;
}

bool Level::containsAnyLiquid(const AABB& aabb)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
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
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);

				if (!Tile::tiles[tileID] || Tile::tiles[tileID]->m_pMaterial != pMtl)
					continue;

				TileData data = getData(pos);
				
				float height;
				if (data <= 7)
					height = (float(pos.y) + 1.0f) + float(data) / 8.0f;
				else
					height = float(pos.y + 1);

				if (aabb.min.y <= height)
					return true;
			}

	return false;
}

bool Level::containsMaterial(const AABB& aabb, const Material* pMtl)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;

	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
				if (Tile::tiles[tileID] && Tile::tiles[tileID]->m_pMaterial == pMtl)
					return true;
			}

	return false;
}

bool Level::checkAndHandleWater(const AABB& aabb, const Material* pMtl, Entity* pEnt)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;

	Vec3 v;
	
	bool bInWater = false;
	TilePos pos(aabb.min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
	{
		for (pos.y = min.y; pos.y < max.y; pos.y++)
		{
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				Tile* pTile = Tile::tiles[getTile(pos)];
				if (!pTile || pTile->m_pMaterial != pMtl)
					continue;

				TileData data = getData(pos);
				int level = data <= 7 ? data + 1 : 1;
				if (float(max.y) >= float(pos.y + 1) - float(level) / 9.0f)
				{
					pTile->handleEntityInside(this, pos, pEnt, v);
					bInWater = true;
				}
			}
		}
	}

	if (v.length() > 0.0f)
	{
		pEnt->m_vel += v * 0.004f;
	}

	return bInWater;
}

const TilePos& Level::getSharedSpawnPos() const
{
	return m_pLevelData->getSpawn();
}

TileID Level::getTopTile(const TilePos& pos) const
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(TilePos(pos.x, y + 1, pos.z)); y++);
	return getTile(TilePos(pos.x, y, pos.z));
}

int Level::getTopTileY(const TilePos& pos) const
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(TilePos(pos.x, y + 1, pos.z)); y++);
	return y;
}

int Level::getTopSolidBlock(const TilePos& tilePos) const
{
	LevelChunk* pChunk = getChunkAt(tilePos);
	if (!pChunk)
		return C_MAX_Y;

	TilePos pos(tilePos);
	pos.y = C_MAX_Y - 1;
	while (true)
	{
		if (!getMaterial(pos)->blocksMotion())
			break;
		if (!pos.y)
			return -1;
		pos.y--;
	}

	ChunkTilePos ctp(pos);
	if (ctp.y <= C_MIN_Y)
		return 0;

	while (true)
	{
		TileID tile = pChunk->getTile(ctp);
		if (tile)
		{
			if (Tile::tiles[tile]->m_pMaterial->blocksMotion())
				return ctp.y + 1;
			
			if (Tile::tiles[tile]->m_pMaterial->isLiquid())
				break;
		}
		if (!--ctp.y)
			return -1;
	}

	return ctp.y + 1;
}

void Level::validateSpawn()
{
	if (m_pLevelData->getYSpawn() <= 0)
		m_pLevelData->setYSpawn(C_MAX_Y / 2);

	TilePos spawn(m_pLevelData->getSpawn());
#ifndef ORIGINAL_CODE
	int nAttempts = 0;
#endif

	// @TODO: fix these gotos
	if (getTopTile(spawn))
		goto label_10;

	TileID tile;

	do
	{
		// While the spawn isn't valid
		do
		{
			spawn.x += m_random.nextInt(8) - m_random.nextInt(8);
			spawn.z += m_random.nextInt(8) - m_random.nextInt(8);

			// avoid spawning near world border
			if (spawn.x < 4) spawn.x += 8;
			if (spawn.z < 4) spawn.z += 8;
			if (spawn.x > C_MAX_CHUNKS_X * 16 - 4) spawn.x -= 8;
			if (spawn.z > C_MAX_CHUNKS_Z * 16 - 4) spawn.z -= 8;

		#ifndef ORIGINAL_CODE
			nAttempts++;
			if (nAttempts >= 100000)
				goto _failure;
		#endif
		}
		while (!getTopTile(spawn));

	label_10:
		tile = getTopTile(spawn);
	}
	while (tile == Tile::invisible_bedrock->m_ID);

	m_pLevelData->setXSpawn(spawn.x);
	m_pLevelData->setZSpawn(spawn.z);

#ifndef ORIGINAL_CODE
	return;

_failure:

	/*
	m_pLevelData->m_spawnPos.x = C_MAX_CHUNKS_X * 16 / 2;
	m_pLevelData->m_spawnPos.z = C_MAX_CHUNKS_X * 16 / 2;
	m_pLevelData->m_spawnPos.y = C_MAX_Y;
	*/

	m_pLevelData->setSpawn(TilePos(0, 32, 0));

	LOG_W("Failed to validate spawn point, using (%d, %d, %d)", m_pLevelData->getXSpawn(), m_pLevelData->getYSpawn(), m_pLevelData->getZSpawn());

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

		LevelChunk* chunk = getChunk(ent->m_chunkPos);
		if (chunk) chunk->removeEntity(ent);

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
		LOG_W("Entity %d already exists. Replacing...", pEnt->hashCode());
		removeEntity(pOldEnt);
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

void Level::loadPlayer(Player& player)
{
	const CompoundTag* tag = m_pLevelData->getLoadedPlayerTag();
	if (tag)
	{
		player.load(*tag);
		m_pLevelData->setLoadedPlayerTag(nullptr);
		//addEntity(&player);
	}
	m_pLevelData->setLoadedPlayerTo(player);

	// 0.2.1 had us only adding the player if LevelData had a CompoundTag
	// who cares if it doesn't?
	addEntity(&player);
}

void Level::prepare()
{
	while (m_pChunkSource->tick());
}

void Level::saveLevelData()
{
	m_pLevelStorage->saveLevelData(m_pLevelData, &m_players);
}

void Level::savePlayerData()
{
	m_pLevelStorage->savePlayerData(*m_pLevelData, m_players);
}

void Level::saveAllChunks()
{
	m_pChunkSource->saveAll();
}

void Level::saveGame()
{
	if (m_pLevelStorage)
	{
		m_pLevelStorage->saveGame(this);
		saveLevelData();
	}
}

void Level::loadEntities()
{
	if (m_pLevelStorage)
	{
		m_pLevelStorage->loadEntities(this);
	}
}

void Level::sendEntityData()
{
	if (!m_pRakNetInstance)
		return;

	// Inlined on 0.2.1, god bless PerfTimer
	for (EntityVector::iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		Entity* ent = *it;
		SynchedEntityData& data = ent->getEntityData();
		if (data.isDirty())
			m_pRakNetInstance->send(new SetEntityDataPacket(ent->m_EntityID, data));
	}
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
		if (m_pDimension->isValidSpawn(TilePos(spawnX, 64, spawnZ)))
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

	m_pLevelData->setSpawn(TilePos(spawnX, 64, spawnZ));

	m_bCalculatingInitialSpawn = false;

#ifndef ORIGINAL_CODE
	return;

_failure:

	// m_pLevelData->setSpawn(C_MAX_CHUNKS_X * 16 / 2, C_MAX_Y, C_MAX_CHUNKS_X * 16 / 2);

	m_pLevelData->setSpawn(TilePos(0, 32, 0));

	LOG_W("Failed to validate spawn point, using (%d, %d, %d)", m_pLevelData->getXSpawn(), m_pLevelData->getYSpawn(), m_pLevelData->getZSpawn());

	return;
#endif
}

bool Level::canSeeSky(const TilePos& pos) const
{
	LevelChunk* pChunk = getChunk(pos);

	//@BUG: no nullptr check
#ifndef ORIGINAL_CODE
	if (!pChunk)
		return true;
#endif

	return pChunk->isSkyLit(pos);
}

Vec3 Level::getSkyColor(Entity* pEnt, float f) const
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

Vec3 Level::getFogColor(float f) const
{
	return m_pDimension->getFogColor(getTimeOfDay(f), f);
}

Vec3 Level::getCloudColor(float f) const
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

bool Level::isUnobstructed(AABB* aabb) const
{
	EntityVector entities = getEntities(nullptr, *aabb);
	if (entities.size() <= 0)
		return true;

	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		Entity* pEnt = *it;
		if (!pEnt->m_bRemoved && pEnt->m_bBlocksBuilding)
            return false;
	}

	return true;
}

bool Level::mayInteract(Player* player, const TilePos& pos) const
{
	return true;
}

bool Level::mayPlace(TileID tile, const TilePos& pos, bool b) const
{
	TileID oldTile = getTile(pos);

	Tile *pTile = Tile::tiles[tile], *pOldTile = Tile::tiles[oldTile];

	if (pTile == nullptr)
		return false;

	AABB* aabb = pTile->getAABB(this, pos);

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

	return pTile->mayPlace(this, pos);
}

void Level::broadcastEntityEvent(const Entity& entity, Entity::EventType::ID eventId)
{
	if (m_bIsClientSide)
		return;

	m_pRakNetInstance->send(new EntityEventPacket(entity.m_EntityID, eventId));
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
		if (!b && t.m_delay > m_pLevelData->getTime())
			break;

		if (hasChunksAt(t.field_4 - 8, t.field_4 + 8))
		{
			TileID tile = getTile(t.field_4);
			if (tile == t.field_10 && tile > 0)
				Tile::tiles[tile]->tick(this, t.field_4, &m_random);
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
		LevelChunk* pChunk = getChunk(pos);

		for (int i = 0; i < 80; i++)
		{
			m_randValue = m_randValue * 3 + m_addend;
			int rand = m_randValue >> 2;

			TilePos tilePos(
				(rand)       & 15,
				(rand >> 16) & 127,
				(rand >> 8)  & 15);

			TileID tile = pChunk->getTile(tilePos);
			if (Tile::shouldTick[tile])
				Tile::tiles[tile]->tick(this, tilePos + pos, &m_random);
		}
	}
}

void Level::tick(Entity* pEnt, bool b)
{
	TilePos tilePos(pEnt->m_pos);

	if (b)
	{
		if (!hasChunksAt(TilePos(tilePos.x - 32, 0, tilePos.z - 32), TilePos(tilePos.x + 32, 128, tilePos.z + 32)))
		{
			LOG_W("Not updating entity %d because we don't have chunks around it loaded", pEnt->m_EntityID);
			return;
		}

		pEnt->m_posPrev = pEnt->m_pos;
		pEnt->m_oRot = pEnt->m_rot;

		if (pEnt->m_bInAChunk)
			pEnt->tick();
	}
	else
	{
		pEnt->m_posPrev = pEnt->m_pos;
		pEnt->m_oRot = pEnt->m_rot;
	}

	ChunkPos cp(pEnt->m_pos);

	if (!pEnt->m_bInAChunk || cp != pEnt->m_chunkPos)
	{
		// move the entity to the new chunk
		if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkPos))
		{
			getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);
		}

		if (hasChunk(cp))
		{
			pEnt->m_bInAChunk = true;
			getChunk(cp)->addEntity(pEnt);
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
	updateSkyDarken();

	setTime(getTime() + 1);
#endif

	tickPendingTicks(false);
	tickTiles();

	sendEntityData();
}

void Level::tickEntities()
{
	// inlined in the original
	removeAllPendingEntityRemovals();

	for (int i = 0; i<int(m_entities.size()); i++)
	{
		Entity* pEnt = m_entities[i];

		if (!pEnt->m_bRemoved)
		{
			tick(pEnt);
		}
		else if (!pEnt->isPlayer() || pEnt->m_bForceRemove)
		{
			if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkPos))
				getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);

			m_entities.erase(m_entities.begin() + i);
			i--;

			entityRemoved(pEnt);
			delete pEnt;
		}
	}
}

HitResult Level::clip(Vec3 v1, Vec3 v2, bool flag) const
{
	TilePos tp1(v1), tp2(v2);
	int counter = 200;
	while (counter-- >= 0)
	{
		if (tp1 == tp2)
			break;

		float xd = 999.0f;
		float yd = 999.0f;
		float zd = 999.0f;
		if (tp2.x > tp1.x) xd = (float)tp1.x + 1.0f;
		if (tp2.x < tp1.x) xd = (float)tp1.x + 0.0f;
		if (tp2.y > tp1.y) yd = (float)tp1.y + 1.0f;
		if (tp2.y < tp1.y) yd = (float)tp1.y + 0.0f;
		if (tp2.z > tp1.z) zd = (float)tp1.z + 1.0f;
		if (tp2.z < tp1.z) zd = (float)tp1.z + 0.0f;
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
				hitSide = tp2.z <= tp1.z ? Facing::SOUTH : Facing::NORTH;
				v1.x = v1.x + (float)(xl * ze);
				v1.y = v1.y + (float)(yl * ze);
				v1.z = zd;
			}
			else
			{
				hitSide = (tp2.y <= tp1.y) ? Facing::UP : Facing::DOWN;
				v1.x = v1.x + (float)(xl * ye);
				v1.y = yd;
				v1.z = v1.z + (float)(zl * ye);
			}
		}
		else
		{
			hitSide = tp2.x <= tp1.x ? Facing::EAST : Facing::WEST;
			v1.x = xd;
			v1.y = v1.y + (float)(yl * xe);
			v1.z = v1.z + (float)(zl * xe);
		}

		Vec3 hitVec(v1);

		// Correct the hit positions for each vector
		hitVec.x = (float)Mth::floor(v1.x);
		tp1.x = (int)hitVec.x;
		if (hitSide == Facing::EAST)
		{
			tp1.x--;
			hitVec.x += 1.0;
		}


		hitVec.y = (float)Mth::floor(v1.y);
		tp1.y = (int)hitVec.y;
		if (hitSide == Facing::UP)
		{
			tp1.y--;
			hitVec.y += 1.0;
		}


		hitVec.z = (float)Mth::floor(v1.z);
		tp1.z = (int)hitVec.z;
		if (hitSide == Facing::SOUTH)
		{
			tp1.z--;
			hitVec.z += 1.0;
		}

		TileID tile = getTile(tp1);
		int    data = getData(tp1);
		Tile* pTile = Tile::tiles[tile];

		if (tile > 0 && pTile->mayPick(data, false))
		{
			HitResult hr = pTile->clip(this, tp1, v1, v2);
			if (hr.isHit())
				return hr;
		}
	}

	return HitResult();
}

HitResult Level::clip(const Vec3& a, const Vec3& b) const
{
	return clip(a, b, false);
}

void Level::addToTickNextTick(const TilePos& tilePos, int d, int delay)
{
	TickNextTickData tntd(tilePos, d);
	if (m_bInstantTicking)
	{
		// @NOTE: Don't know why this check wasn't just placed at the beginning.
		if (!hasChunksAt(tilePos, 8))
			return;

		TileID tile = getTile(tntd.field_4);
		if (tile > 0 && tile == tntd.field_10)
			Tile::tiles[tntd.field_10]->tick(this, tntd.field_4, &m_random);
	}
	else
	{
		if (!hasChunksAt(tilePos, 8))
			return;

		if (d > 0)
			tntd.setDelay(delay + getTime());

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

void Level::addParticle(const std::string& name, const Vec3& pos, const Vec3& dir)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->addParticle(name, pos, dir);
	}
}

void Level::playSound(Entity* entity, const std::string& name, float volume, float pitch)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, Vec3(entity->m_pos.x, entity->m_pos.y - entity->m_heightOffset, entity->m_pos.z), volume, pitch);
	}
}

void Level::playSound(const Vec3& pos, const std::string& name, float a, float b)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, pos, a, b);
	}
}

void Level::animateTick(const TilePos& pos)
{
	Random random;

	// frequency is 1000 on JE, 100 on PE
	for (int i = 0; i < 100; i++)
	{
		TilePos aPos(pos.x + m_random.nextInt(16) - m_random.nextInt(16),
					 pos.y + m_random.nextInt(16) - m_random.nextInt(16),
					 pos.z + m_random.nextInt(16) - m_random.nextInt(16));
		TileID tile = getTile(aPos);
		if (tile > 0)
			Tile::tiles[tile]->animateTick(this, aPos, &random);
	}
}

float Level::getSeenPercent(Vec3 vec, AABB aabb) const
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

void Level::explode(Entity* entity, const Vec3& pos, float power)
{
	explode(entity, pos, power, false);
}

void Level::explode(Entity* entity, const Vec3& pos, float power, bool bIsFiery)
{
	Explosion expl(this, entity, pos, power);
	expl.setFiery(bIsFiery);
	expl.explode();
	expl.addParticles();
}

void Level::addEntities(const EntityVector& entities)
{
	m_entities.insert(m_entities.end(), entities.begin(), entities.end());

	for (EntityVector::iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		Entity* pEnt = *it;
		entityAdded(pEnt);
	}
}

// @UNUSED
void Level::ensureAdded(Entity* entity)
{
	ChunkPos chunkPos(Mth::floor(entity->m_pos.x / 16.0f),
		              Mth::floor(entity->m_pos.z / 16.0f));

	// force the chunk to be resolved
	ChunkPos cp;
	for (cp.x = chunkPos.x - 2; cp.x <= chunkPos.x + 2; cp.x++)
		for (cp.z = chunkPos.z - 2; cp.z <= chunkPos.z + 2; cp.z++)
			getChunk(cp);

	EntityVector::iterator result = std::find(m_entities.begin(), m_entities.end(), entity);
	if (result == m_entities.end())
		m_entities.push_back(entity);
}

bool Level::extinguishFire(Player* player, const TilePos& pos, Facing::Name face)
{
	TilePos p(pos.relative(face));

	if (getTile(p) == Tile::fire->m_ID)
		return setTile(p, TILE_AIR);

	return false;
}

int Level::findPath(Path* path, Entity* ent, Entity* target, float f) const
{
	TilePos tp(ent->m_pos);
	Region reg(this, tp - int(f + 16), tp + int(f + 16));

	m_pPathFinder->setLevel(&reg);
	return m_pPathFinder->findPath(*path, ent, target, f);

	// wtf?
	// return 1;
}

int Level::findPath(Path* path, Entity* ent, const TilePos& pos, float f) const
{
	TilePos tp(ent->m_pos);
	Region reg(this, tp - int(f + 8), tp + int(f + 8));
	
	m_pPathFinder->setLevel(&reg);
	return m_pPathFinder->findPath(*path, ent, pos, f);

	// wtf?
	// return 1;
}

int Level::getLightDepth(const TilePos& pos) const
{
	return getChunk(pos)->getHeightmap(pos);
}

float Level::getStarBrightness(float f) const
{
	float ca = Mth::cos(getSunAngle(f));
	float cb = 1.0f - (0.75f + 2 * ca);

	if (cb < 0.0f)
		cb = 0.0f;
	if (cb > 1.0f)
		cb = 1.0f;

	return cb * cb * 0.5f;
}

float Level::getSunAngle(float f) const
{
	return (float(M_PI) * getTimeOfDay(f)) * 2;
}
