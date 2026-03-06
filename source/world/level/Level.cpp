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
#include "network/packets/ExplodePacket.hpp"
#include "network/packets/SetTimePacket.hpp"
#include "world/entity/MobSpawner.hpp"
#include "world/level/TileSource.hpp"
#include "common/threading/BackgroundQueuePool.hpp"

#include "Explosion.hpp"

Level::Level(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion)
{
	m_bIsClientSide = false;
	field_30 = 0;
    m_difficulty = 2; // Java has no actual default, it just always pulls from Options. Putting 2 here just so there's no chance of mobs getting despawned accidentally.
	m_pRakNetInstance = nullptr;
	m_pLevelStorage = pStor;
	m_randValue = 42184323;
	m_addend = 1013904223;
	m_processingLightUpdates = false;
	field_B0C = 0;

	m_random.setSeed(1); // initialize with a seed of 1

	LevelData* pData = m_pLevelStorage->prepareLevel(this);

	field_B0C = pData == nullptr;

	if (!pData)
		m_pLevelData = new LevelData(settings, name, storageVersion);
	else
		m_pLevelData = pData;

	m_pPathFinder = new PathFinder();
	m_pMobSpawner = new MobSpawner();
}

Level::~Level()
{
	{
		std::vector<BackgroundQueue*> streamingQueues = BackgroundQueuePool::getInstance().getQueuesFor(BackgroundQueuePool::QR_STREAMING);
		for (std::vector<BackgroundQueue*>::iterator iter = streamingQueues.begin(); iter != streamingQueues.end(); iter++)
			(*iter)->flush();
	}

	// TODO: levellistener stuff

	// TODO: player stuff

	BackgroundQueuePool::GetFor(BackgroundQueuePool::QR_MAIN).flush();

	SAFE_DELETE(m_pPathFinder);
	SAFE_DELETE(m_pMobSpawner);
}

Dimension* Level::getDimension(DimensionId type) const
{
	for (DimensionVector::const_iterator iter = m_dimensions.begin(); iter != m_dimensions.end(); iter++)
	{
		Dimension* dimension = *iter;
		if (dimension->getId() == type)
			return dimension;
	}

	return nullptr;
}

Entity* Level::getEntity(Entity::ID id) const
{
	for (DimensionVector::const_iterator iter = m_dimensions.begin(); iter != m_dimensions.end(); iter++)
	{
		Dimension* dimension = *iter;
		Entity* entity = dimension->getEntity(id);
		if (entity)
			return entity;
	}

	return nullptr;
}

void Level::getEntities(DimensionId dimensionId, const EntityType& type, const AABB& aabb, std::vector<Entity*>& output) const
{
	long lowerXBound = floor((aabb.min.x - 2.0f) / 16);
	long lowerZBound = floor((aabb.min.z - 2.0f) / 16);
	long upperXBound = floor((aabb.max.x + 2.0f) / 16);
	long upperZBound = floor((aabb.max.z + 2.0f) / 16);

	ChunkSource* chunkSource = getDimension(dimensionId)->getChunkSource();

	for (int z = lowerZBound; z <= upperZBound; z++)
	{
		for (int x = lowerXBound; x <= upperXBound; x++)
		{
			LevelChunk* chunk = chunkSource->getAvailableChunk(ChunkPos(x, z));
			if (chunk)
				chunk->getEntities(type, aabb, output);
		}
	}

	if (type == EntityType::PLAYER)
	{
		for (std::vector<Player*>::const_iterator iter = m_players.begin(); iter != m_players.end(); iter++)
		{
			Player* player = *iter;
			if (player->m_hitbox.intersect(aabb))
				output.push_back(player);
		}
	}
}

void Level::updateLights()
{
	if (!m_lightUpdates.empty() && !m_processingLightUpdates)
	{
		m_processingLightUpdates = true; // NOTE: presumably meant to be here, not in PE

		BackgroundQueuePool::getInstance().getMain().queue(
			std::bind(&Level::_updateLightsTask, this),
			BackgroundQueue::NOP,
			1
		);
	}
}

bool Level::_updateLightsTask()
{
	while (!m_lightUpdates.empty())
	{
		LightUpdate& update = m_lightUpdates.back();
		update.updateFast();
		m_lightUpdates.pop_back();
	}

	m_processingLightUpdates = false;
	return true;
}

int32_t Level::setTime(int32_t time)
{
	const int totalTime = 192000;

	int32_t newTime = time % totalTime;
	m_pLevelData->setTime(newTime);
	return newTime < m_lastGameTimeSync ? (totalTime - m_lastGameTimeSync + newTime) : (newTime - m_lastGameTimeSync);
}

void Level::_syncTime(int32_t time)
{
	if (m_bIsClientSide)
		return;

	broadcastAll(new SetTimePacket(time));
}

void Level::entityAdded(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityAdded(pEnt);
	}
}

void Level::entityRemoved(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityRemoved(pEnt);
	}
}

void Level::levelEvent(const LevelEvent& event)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->levelEvent(event);
	}
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

bool Level::checkAndHandleWater(const AABB& aabb, const Material* pMtl, Entity* pEnt)
{
	TileSource* source = pEnt->m_tileSource;

	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!source->hasChunksAt(min, max))
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
				Tile* pTile = Tile::tiles[source->getTile(pos)];
				if (!pTile || pTile->m_pMaterial != pMtl)
					continue;

				TileData data = source->getData(pos);
				int level = data <= 7 ? data + 1 : 1;
				if (float(max.y) >= float(pos.y + 1) - float(level) / 9.0f)
				{
					pTile->handleEntityInside(source, pos, pEnt, v);
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

void Level::removeAllPendingEntityRemovals()
{
	/*Util::removeAll(m_entities, m_pendingEntityRemovals);

	for (EntityVector::iterator it = m_pendingEntityRemovals.begin(); it != m_pendingEntityRemovals.end(); it++)
	{
		Entity* ent = *it;
		ent->removed();

		LevelChunk* chunk = getChunk(ent->m_chunkPos);
		if (chunk) chunk->removeEntity(ent);

		entityRemoved(ent);

		//@BUG: MEMORY LEAK -- probably leaking entities here?
	}*/

	m_pendingEntityRemovals.clear();
}

bool Level::addEntity(std::unique_ptr<Entity> entity)
{
	Dimension& dimension = entity->m_tileSource->getDimension();

	if (dimension.hasEntity(*entity))
		return false;

	LevelChunk* chunk = entity->m_tileSource->getChunk(entity->m_pos);
	if (!chunk)
		return false;

	Entity* entityTemp = entity.get();

	chunk->addEntity(std::move(entity));
	entityTemp->m_bInAChunk = true;

	dimension.addEntity(*entityTemp);
	entityAdded(entityTemp);

	return true;
}

bool Level::addEntity(Entity* entity)
{
	return addEntity(std::unique_ptr<Entity>(entity));
}

void Level::removeEntity(Entity& entity)
{
	// TODO: this check needs to go away from here, i need to find where this is originally called for player
	if (entity.isPlayer())
		Util::remove(m_players, (Player*)&entity);

	Dimension& dimension = entity.m_tileSource->getDimension();

	dimension.removeEntity(entity);
	entityRemoved(&entity);
}

void Level::removeEntity(std::unique_ptr<Entity>&& entity)
{
	removeEntity(*entity);
	m_pendingEntityRemovals.push_back(std::move(entity));
}

void Level::removeEntity(Entity*& entity)
{
	removeEntity(std::unique_ptr<Entity>(entity));
	entity = nullptr;
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
	if (addEntity(std::make_unique<Entity>(player)))
		m_players.push_back(&player);
}

void Level::saveLevelData()
{
	m_pLevelStorage->saveLevelData(m_pLevelData, &m_players);
}

void Level::savePlayerData()
{
	m_pLevelStorage->savePlayerData(*m_pLevelData, m_players);
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

void Level::broadcastAll(Packet* packet)
{
	assert(!m_bIsClientSide);

	if (m_pRakNetInstance)
	{
		m_pRakNetInstance->send(packet);
	}
}

void Level::broadcastToAllInRange(Packet* packet, const Vec3& pos, float range, Player* avoid)
{
	assert(!m_bIsClientSide);

	if (m_pRakNetInstance)
	{
		for (size_t i = 0; i < m_players.size(); i++)
		{
			Player* pPlayer = m_players[i];
			if (!pPlayer)
				continue;

			if (pPlayer != avoid)
			{
				Vec3 diff = pos - pPlayer->m_pos;
				if (diff.lengthSqr() < range * range)
					m_pRakNetInstance->send(pPlayer->m_guid, *packet);
			}
		}
	}
	delete packet;
}

void Level::broadcastEntityEvent(const Entity& entity, Entity::EventType::ID eventId)
{
	if (m_bIsClientSide || !m_pRakNetInstance)
		return;

	m_pRakNetInstance->send(new EntityEventPacket(entity.m_EntityID, eventId));
}

void Level::addListener(LevelListener* listener)
{
	m_listeners.push_back(listener);
}

void Level::removeListener(LevelListener* listener)
{
	std::vector<LevelListener*>::iterator iter = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (iter != m_listeners.end())
		m_listeners.erase(iter);
}

void Level::tick()
{
	tickEntities();

	tickTime();

	for (DimensionVector::iterator it = m_dimensions.begin(); it != m_dimensions.end(); it++)
	{
		Dimension& dimension = **it;
		dimension.tick();
	}

	m_pLevelData->incrementCurrentTick();
}

void Level::tickEntities()
{
	// inlined in the original
	removeAllPendingEntityRemovals();
}

void Level::tickTime()
{
	if (m_bIsClientSide)
		return;

	int32_t newTime = getTime() + 1;
	int32_t timeSinceLastSync = setTime(newTime);
	if (timeSinceLastSync > 255)
	{
		_syncTime(newTime);
		m_lastGameTimeSync = newTime;
	}
}

void Level::takePicture(TripodCamera* pCamera, Entity* pOwner)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->takePicture(pCamera, pOwner);
	}
}

void Level::addParticle(const std::string& name, const Vec3& pos, const Vec3& dir)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->addParticle(name, pos, dir);
	}
}

void Level::playSound(Entity* entity, const std::string& name, float volume, float pitch)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, Vec3(entity->m_pos.x, entity->m_pos.y - entity->m_heightOffset, entity->m_pos.z), volume, pitch);
	}
}

void Level::playSound(const Vec3& pos, const std::string& name, float a, float b)
{
	for (std::vector<LevelListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, pos, a, b);
	}
}

void Level::animateTick(Entity* entity)
{
	TileSource* source = entity->m_tileSource;

	// frequency is 1000 on JE, 100 on PE
	for (int i = 0; i < 100; i++)
	{
		TilePos aPos(entity->m_pos.x + m_random.nextInt(16) - m_random.nextInt(16),
			entity->m_pos.y + m_random.nextInt(16) - m_random.nextInt(16),
			entity->m_pos.z + m_random.nextInt(16) - m_random.nextInt(16));
		TileID tile = source->getTile(aPos);
		if (tile > 0)
			Tile::tiles[tile]->animateTick(source, aPos, &m_random2);
	}
}

void Level::explode(Entity* entity, const Vec3& pos, float power)
{
	explode(entity, pos, power, false);
}

void Level::explode(Entity* entity, const Vec3& pos, float power, bool bIsFiery)
{
	Explosion expl(entity->getTileSource(), entity, pos, power);
	expl.setFiery(bIsFiery);
	expl.explode();
	expl.addParticles();

#if NETWORK_PROTOCOL_VERSION >= 3
	if (!m_bIsClientSide)
	{
		broadcastToAllInRange(new ExplodePacket(pos, power), pos, 64.0f);
	}
#endif
}

bool Level::extinguishFire(TileSource& source, const TilePos& pos, Facing::Name face)
{
	TilePos p(pos.relative(face));

	if (source.getTile(p) == Tile::fire->m_ID)
		return source.setTile(p, TILE_AIR);

	return false;
}

int Level::findPath(Path* path, Entity* ent, Entity* target, float f) const
{
	TilePos tp(ent->m_pos);

	return m_pPathFinder->findPath(*path, ent, target, f);
}

int Level::findPath(Path* path, Entity* ent, const TilePos& pos, float f) const
{
	TilePos tp(ent->m_pos);
	
	return m_pPathFinder->findPath(*path, ent, pos, f);
}

void Level::addListener(LevelListener& listener)
{
	m_listeners.push_back(&listener);
}

void Level::removeListener(LevelListener& listener)
{
	Listeners::iterator it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
	if (it != m_listeners.end())
		m_listeners.erase(it);
}

void Level::onChunkLoaded(LevelChunk& chunk)
{
	// @TODO: this
	throw std::bad_cast();
}
