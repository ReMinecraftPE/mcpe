/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
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
#include "world/level/storage/LevelSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"
#include "Dimension.hpp"
#include "LevelListener.hpp"
#include "TickNextTickData.hpp"
#include "LevelEvent.hpp"

class Dimension;
class Level;
class LevelListener;
class RakNetInstance;

typedef std::vector<Entity*> EntityVector;
typedef std::vector<AABB> AABBVector;

class Level : public LevelSource
{
public:
	Level(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion = LEVEL_STORAGE_VERSION_DEFAULT, Dimension* pDimension = nullptr);
	~Level();

private:
	Player* _getNearestPlayer(const Vec3&, float, bool) const;

protected:
	// @NOTE: LevelListeners do NOT get updated here
	void _setTime(int32_t time) { m_pLevelData->setTime(time); }

public:
	// TODO
	TileID getTile(const TilePos& pos) const override;
	float getBrightness(const TilePos& pos) const override;
	TileData getData(const TilePos& pos) const override;
	Material* getMaterial(const TilePos& pos) const override;
	bool isSolidTile(const TilePos& pos) const override;

	ChunkSource* getChunkSource() const;
	virtual ChunkSource* createChunkSource();
	LevelChunk* getChunk(const ChunkPos& pos) const;
	LevelChunk* getChunkAt(const TilePos& pos) const;
	int getRawBrightness(const TilePos& pos) const;
	int getRawBrightness(const TilePos& pos, bool b) const;
	int getBrightness(const LightLayer&, const TilePos& pos) const;
	void setBrightness(const LightLayer&, const TilePos& pos, int brightness);
	int getSeaLevel() const { return 63; }
	int getSeed() const { return m_pLevelData->getSeed(); }
	int32_t getTime() const { return m_pLevelData->getTime(); }
	void setTime(int32_t time);
	GameType getDefaultGameType() const { return m_pLevelData->getGameType(); }
	int getHeightmap(const TilePos& pos);
	bool isDay() const;
	bool isSkyLit(const TilePos& pos) const;
	bool isEmptyTile(const TilePos& pos) const;
	bool hasChunkAt(const TilePos& pos) const;
	bool hasChunk(const ChunkPos& pos) const;
	bool hasChunksAt(const TilePos& min, const TilePos& max) const;
	bool hasChunksAt(const TilePos& pos, int rad) const;
	float getTimeOfDay(float f) const;
	int getSkyDarken(float f) const;
	void updateSkyDarken();
	bool updateSkyBrightness();
	void setUpdateLights(bool b);
	bool updateLights();
	void updateLight(const LightLayer&, const TilePos& tilePos1, const TilePos& tilePos2);
	void updateLight(const LightLayer&, const TilePos& tilePos1, const TilePos& tilePos2, bool);
	void updateLightIfOtherThan(const LightLayer&, const TilePos& pos, int);
	bool setTileAndDataNoUpdate(const TilePos& pos, TileID tile, TileData data);
	bool setTileNoUpdate(const TilePos& pos, TileID tile);
	bool setDataNoUpdate(const TilePos& pos, TileData data);
	bool setTileAndData(const TilePos& pos, TileID tile, TileData data, TileChange::UpdateFlags updateFlags = TileChange::UPDATE_ALL);
	bool setTile(const TilePos& pos, TileID tile, TileChange::UpdateFlags updateFlags = TileChange::UPDATE_ALL);
	bool setData(const TilePos& pos, TileData data, TileChange::UpdateFlags updateFlags = TileChange::UPDATE_ALL);
	void sendTileUpdated(const TilePos& pos);
	void tileUpdated(const TilePos& pos, TileID tile);
	void updateNeighborsAt(const TilePos& pos, TileID tile);
	void neighborChanged(const TilePos& pos, TileID tile);
	void setTilesDirty(const TilePos& min, const TilePos& max);
	void entityAdded(Entity* pEnt);
	void entityRemoved(Entity* pEnt);
	void levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data = 0);
	void lightColumnChanged(int x, int z, int y1, int y2);
	bool containsFireTile(const AABB&);
	bool containsAnyLiquid(const AABB&);
	bool containsLiquid(const AABB&, const Material *pMtl);
	bool containsMaterial(const AABB&, const Material *pMtl);
	bool checkAndHandleWater(const AABB&, const Material* pMtl, Entity* pEnt);
	const TilePos& getSharedSpawnPos() const;
	void validateSpawn();
	TileID getTopTile(const TilePos& pos) const;
	int getTopTileY(const TilePos& pos) const;
	int getTopSolidBlock(const TilePos& tilePos) const;
	void loadPlayer(Player&);
	bool addEntity(Entity*);
	bool removeEntity(Entity*);
	void removeEntities(const EntityVector&);
	void removeAllPendingEntityRemovals();
	void prepare();
	void saveLevelData();
	void savePlayerData();
	void saveAllChunks();
	void saveGame();
	void loadEntities();
	void sendEntityData();
	void setInitialSpawn();
	void setSpawnPos(const TilePos& pos) { m_pLevelData->setSpawn(pos); }
	void setSpawnSettings(bool a, bool b) { }
	bool canSeeSky(const TilePos& pos) const;
	Vec3 getSkyColor(Entity* pEnt, float f) const;
	Vec3 getFogColor(float f) const;
	Vec3 getCloudColor(float f) const;
	bool isUnobstructed(AABB*) const;
	bool mayInteract(Player* player, const TilePos& pos) const;
	bool mayPlace(TileID tid, const TilePos& pos, bool b) const;
	void broadcastEntityEvent(const Entity& entity, Entity::EventType::ID eventId);
	void removeListener(LevelListener*);
	void addListener(LevelListener*);
	void tick(Entity*, bool);
	void tick(Entity*);
	virtual void tick();
	void tickPendingTicks(bool b);
	void tickTiles();
	void tickEntities();
	void addToTickNextTick(const TilePos& tilePos, int, int);
	void takePicture(TripodCamera* pCamera, Entity* pOwner);
	void addParticle(const std::string& name, const Vec3& pos, const Vec3& dir = Vec3::ZERO);
	void playSound(Entity*, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void playSound(const Vec3& pos, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void animateTick(const TilePos& pos);
	float getSeenPercent(Vec3, AABB) const;
	void explode(Entity*, const Vec3& pos, float power);
	void explode(Entity*, const Vec3& pos, float power, bool bIsFiery);
	void addEntities(const EntityVector& entities);
	void ensureAdded(Entity* entity);
	bool extinguishFire(Player* player, const TilePos& pos, Facing::Name face);
	int findPath(Path* path, Entity* ent1, Entity* ent2, float f) const;
	int findPath(Path* path, Entity* ent, const TilePos& pos, float f) const;
	int getLightDepth(const TilePos& pos) const;
	float getStarBrightness(float f) const;
	float getSunAngle(float f) const;
	void swap(const TilePos& pos1, const TilePos& pos2);

	HitResult clip(const Vec3& a, const Vec3& b) const;
	HitResult clip(Vec3 a, Vec3 b, bool c) const;
	Entity* getEntity(Entity::ID id) const;
	const EntityVector* getAllEntities() const;
	EntityVector getEntities(Entity* pAvoid, const AABB&) const;
	BiomeSource* getBiomeSource() const override;
	LevelStorage* getLevelStorage() const { return m_pLevelStorage; }
	const LevelData* getLevelData() const { return m_pLevelData; }
	AABBVector* getCubes(const Entity* pEnt, const AABB& aabb);
	std::vector<LightUpdate>* getLightsToUpdate();
	Player* getNearestPlayer(const Entity&, float) const;
	Player* getNearestPlayer(const Vec3& pos, float, bool) const;
	Player* getNearestAttackablePlayer(const Entity&, float) const;
	Player* getNearestAttackablePlayer(const Vec3& pos, float, const Entity*) const;

	// unused redstone stuff
	int getSignal(const TilePos& pos, Facing::Name face) const;
	int getDirectSignal(const TilePos& pos, Facing::Name face) const;
	bool hasDirectSignal(const TilePos& pos) const;
	bool hasNeighborSignal(const TilePos& pos) const;

#ifdef ENH_IMPROVED_SAVING
	void saveUnsavedChunks();
#endif

private:
	LevelData* m_pLevelData;

protected:
	int m_randValue;
	int m_addend;

public:
	AABBVector m_aabbs;
	bool m_bInstantTicking;
	bool m_bIsClientSide; // if the level is controlled externally by a server.
	bool m_bPostProcessing;
	EntityVector m_entities;
	std::vector<Player*> m_players;
	int m_skyDarken;
	uint8_t field_30;
	Dimension* m_pDimension;
    int m_difficulty; // @TODO: Difficulty enum
	Random m_random;
	RakNetInstance* m_pRakNetInstance;
	bool m_bCalculatingInitialSpawn;
	std::vector<LevelListener*> m_levelListeners;
	ChunkSource* m_pChunkSource;
	LevelStorage* m_pLevelStorage;
	EntityVector m_pendingEntityRemovals;
	std::set<TickNextTickData> m_pendingTicks;
	std::set<ChunkPos> m_chunksToUpdate;
	std::vector<LightUpdate> m_lightUpdates;
	bool m_bUpdateLights;
	int field_B08;
	uint8_t field_B0C;
	int field_B10;
	PathFinder* m_pPathFinder;
};

