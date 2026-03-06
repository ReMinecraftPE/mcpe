/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
#include <map>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "client/renderer/LightUpdate.hpp"
#include "world/tile/Tile.hpp"
#include "world/entity/Entity.hpp"
#include "world/level/TileChange.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"
#include "Dimension.hpp"
#include "LevelListener.hpp"
#include "LevelEvent.hpp"
#include "TileSourceListener.hpp"

class Dimension;
class Level;
class LevelListener;
class RakNetInstance;
class Packet;
class MobSpawner;

class Level : public LevelListener, public TileSourceListener
{
public:
	typedef std::vector<Dimension*> DimensionVector;

public:
	Level(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion = LEVEL_STORAGE_VERSION_DEFAULT);
	~Level();

private:
	Player* _getNearestPlayer(const Vec3&, float, bool) const;

public:
	int getSeaLevel() const { return 63; }
	int getSeed() const { return m_pLevelData->getSeed(); }
	int32_t getTime() const { return m_pLevelData->getTime(); }
	int32_t setTime(int32_t time); // Returns the time between the new time and the last synchronised time
	GameType getDefaultGameType() const { return m_pLevelData->getGameType(); }
	void updateLights();
	void entityAdded(Entity* pEnt);
	void entityRemoved(Entity* pEnt);
	void levelEvent(const LevelEvent& event);
	bool checkAndHandleWater(const AABB&, const Material* pMtl, Entity* pEnt);
	const TilePos& getSharedSpawnPos() const;
	void loadPlayer(Player&);
	bool addEntity(std::unique_ptr<Entity> entity);
	virtual bool addEntity(Entity* entity);
	virtual void removeEntity(Entity& entity);
	void removeEntity(std::unique_ptr<Entity>&& entity);
	virtual void removeEntity(Entity*& entity);
	void removeAllPendingEntityRemovals();
	void saveLevelData();
	void savePlayerData();
	void saveGame();
	void loadEntities();
	void setSpawnPos(const TilePos& pos) { m_pLevelData->setSpawn(pos); }
	void setSpawnSettings(bool a, bool b) { }
	void broadcastAll(Packet* packet);
	void broadcastToAllInRange(Packet* packet, const Vec3& pos, float range, Player* avoid = nullptr);
	void broadcastEntityEvent(const Entity& entity, Entity::EventType::ID eventId);
	void removeListener(LevelListener*);
	void addListener(LevelListener*);
	virtual void tick();
	void tickEntities();
	void tickTime();
	void takePicture(TripodCamera* pCamera, Entity* pOwner);
	void addParticle(const std::string& name, const Vec3& pos, const Vec3& dir = Vec3::ZERO);
	void playSound(Entity*, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void playSound(const Vec3& pos, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void animateTick(Entity* entity);
	void explode(Entity*, const Vec3& pos, float power);
	void explode(Entity*, const Vec3& pos, float power, bool bIsFiery);
	bool extinguishFire(TileSource& source, const TilePos& pos, Facing::Name face);
	int findPath(Path* path, Entity* ent1, Entity* ent2, float f) const;
	int findPath(Path* path, Entity* ent, const TilePos& pos, float f) const;

	Entity* getEntity(Entity::ID id) const;
	void getEntities(DimensionId dimensionId, const EntityType& type, const AABB& aabb, std::vector<Entity*>& output) const;
	LevelStorage* getLevelStorage() const { return m_pLevelStorage; }
	Dimension* getDimension(DimensionId type) const;
	const LevelData* getLevelData() const { return m_pLevelData; }
	std::vector<LightUpdate>* getLightsToUpdate();
	Player* getNearestPlayer(const Entity&, float) const;
	Player* getNearestPlayer(const Vec3& pos, float, bool) const;
	Player* getNearestAttackablePlayer(const Entity&, float) const;
	Player* getNearestAttackablePlayer(const Vec3& pos, float, const Entity*) const;

	virtual void onChunkLoaded(LevelChunk& chunk);

protected:
	void _syncTime(int32_t time);
	bool _updateLightsTask();

private:
	LevelData* m_pLevelData;

protected:
	int m_randValue;
	int m_addend;

public:
	bool m_bIsClientSide; // if the level is controlled externally by a server.
	DimensionVector m_dimensions;
	std::vector<Player*> m_players;
	uint8_t field_30;
    int m_difficulty; // @TODO: Difficulty enum
	Random m_random;
	Random m_random2;
	RakNetInstance* m_pRakNetInstance;
	std::vector<LevelListener*> m_levelListeners;
	LevelStorage* m_pLevelStorage;
	std::vector<std::unique_ptr<Entity>> m_pendingEntityRemovals;
	std::set<ChunkPos> m_chunksToUpdate;
	std::vector<LightUpdate> m_lightUpdates;
	bool m_processingLightUpdates;
	uint8_t field_B0C;
	int field_B10;
	PathFinder* m_pPathFinder;
	MobSpawner* m_pMobSpawner;
	int32_t m_lastGameTimeSync;
};

