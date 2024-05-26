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

#include "world/tile/Tile.hpp"
#include "world/entity/Entity.hpp"
#include "world/entity/LocalPlayer.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"
#include "Dimension.hpp"
#include "LevelListener.hpp"
#include "TickNextTickData.hpp"
#include "client/renderer/LightUpdate.hpp"

class Dimension;
class Level;
class LevelListener;

typedef std::vector<Entity*> EntityVector;
typedef std::vector<AABB> AABBVector;

class Level : public LevelSource
{
private:
	LevelData m_levelData;

private:
	// @NOTE: LevelListeners do NOT get updated here
	void _setTime(int32_t time) { m_levelData.setTime(time); }

public:
	Level(LevelStorage* pStor, const std::string& str, int32_t seed, int version, Dimension* pDimension = nullptr);
	~Level();

	// TODO
	TileID getTile(int x, int y, int z) const override;
	float getBrightness(int x, int y, int z) const override;
	int getData(int x, int y, int z) const override;
	Material* getMaterial(int x, int y, int z) const override;
	bool isSolidTile(int x, int y, int z) const override;

	ChunkSource* getChunkSource() const;
	ChunkSource* createChunkSource();
	LevelChunk* getChunk(int x, int z) const;
	LevelChunk* getChunkAt(int x, int z) const;
	int getRawBrightness(int x, int y, int z) const;
	int getRawBrightness(int x, int y, int z, bool b) const;
	int getBrightness(const LightLayer&, int x, int y, int z) const;
	void setBrightness(const LightLayer&, int x, int y, int z, int bright);
	int getSeaLevel() const { return 63; }
	int getSeed() const { return m_levelData.getSeed(); }
	int32_t getTime() const { return m_levelData.getTime(); }
	void setTime(int32_t time);
	GameType getDefaultGameType() { return m_levelData.getGameType(); }
	int getHeightmap(int x, int z);
	bool isDay() const;
	bool isSkyLit(int x, int y, int z) const;
	bool isEmptyTile(int x, int y, int z) const;
	bool hasChunkAt(int x, int y, int z) const;
	bool hasChunk(int x, int z) const;
	bool hasChunksAt(int minX, int minY, int minZ, int maxX, int maxY, int maxZ) const;
	bool hasChunksAt(int x, int y, int z, int rad) const;
	bool updateSkyBrightness();
	float getTimeOfDay(float f) const;
	int getSkyDarken(float f) const;
	void setUpdateLights(bool b);
	bool updateLights();
	void updateLight(const LightLayer&, int, int, int, int, int, int);
	void updateLight(const LightLayer&, int, int, int, int, int, int, bool);
	void updateLightIfOtherThan(const LightLayer&, int x, int y, int z, int);
	bool setTileAndDataNoUpdate(int x, int y, int z, TileID tile, int data);
	bool setTileNoUpdate(int x, int y, int z, TileID tile);
	bool setDataNoUpdate(int x, int y, int z, int data);
	bool setTileAndData(int x, int y, int z, TileID tile, int data);
	bool setTile(int x, int y, int z, TileID tile);
	bool setData(int x, int y, int z, int data);
	void sendTileUpdated(int x, int y, int z);
	void tileUpdated(int x, int y, int z, TileID tile);
	void updateNeighborsAt(int x, int y, int z, TileID tile);
	void neighborChanged(int x, int y, int z, TileID tile);
	void setTilesDirty(int x1, int y1, int z1, int x2, int y2, int z2);
	void entityAdded(Entity* pEnt);
	void entityRemoved(Entity* pEnt);
	void lightColumnChanged(int x, int z, int y1, int y2);
	bool containsFireTile(AABB);
	bool containsAnyLiquid(AABB);
	bool containsLiquid(const AABB&, const Material *pMtl);
	bool containsMaterial(const AABB&, const Material *pMtl);
	bool checkAndHandleWater(const AABB&, const Material* pMtl, Entity* pEnt);
	Pos getSharedSpawnPos() const;
	void validateSpawn();
	TileID getTopTile(int x, int z) const;
	int getTopTileY(int x, int z) const;
	int getTopSolidBlock(int x, int z) const;
	void loadPlayer(Player*);
	bool addEntity(Entity*);
	bool removeEntity(Entity*);
	void removeEntities(const EntityVector&);
	void removeAllPendingEntityRemovals();
	void prepare();
	void saveLevelData();
	void savePlayerData();
	void saveAllChunks();
	void setInitialSpawn();
	void setSpawnPos(Pos pos) { m_levelData.setSpawn(pos.x, pos.y, pos.z); }
	void setSpawnSettings(bool a, bool b) { }
	bool canSeeSky(int x, int y, int z) const;
	Vec3 getSkyColor(Entity* pEnt, float f) const;
	Vec3 getFogColor(float f) const;
	Vec3 getCloudColor(float f) const;
	bool isUnobstructed(AABB*) const;
	bool mayInteract(Player* player, int x, int y, int z) const;
	bool mayPlace(TileID tid, int x, int y, int z, bool b) const;
	void removeListener(LevelListener*);
	void addListener(LevelListener*);
	void tick(Entity*, bool);
	void tick(Entity*);
	void tick();
	void tickPendingTicks(bool b);
	void tickTiles();
	void tickEntities();
	void addToTickNextTick(int, int, int, int, int);
	void takePicture(TripodCamera* pCamera, Entity* pOwner);
	void addParticle(const std::string& name, float, float, float, float, float, float);
	void playSound(Entity*, const std::string& name, float volume, float pitch);
	void playSound(float x, float y, float z, const std::string& name, float volume, float pitch);
	void animateTick(int x, int y, int z);
	float getSeenPercent(Vec3, AABB) const;
	void explode(Entity*, float x, float y, float z, float power);
	void explode(Entity*, float x, float y, float z, float power, bool bIsFiery);
	void addEntities(const std::vector<Entity*>& entities);
	void ensureAdded(Entity* entity);
	void extinguishFire(int x, int y, int z, int dir);
	int  findPath(Path* path, Entity* ent1, Entity* ent2, float f) const;
	int  findPath(Path* path, Entity* ent1, int x, int y, int z, float f) const;
	int  getLightDepth(int x, int z) const;
	float getStarBrightness(float f) const;
	float getSunAngle(float f) const;
	void swap(int x1, int y1, int z1, int x2, int y2, int z2);

	HitResult clip(const Vec3& a, const Vec3& b) const;
	HitResult clip(Vec3 a, Vec3 b, bool c) const;
	Entity* getEntity(int id) const;
	const EntityVector* getAllEntities() const;
	EntityVector getEntities(Entity* pAvoid, const AABB&) const;
	BiomeSource* getBiomeSource() const override;
	LevelStorage* getLevelStorage() const { return m_pLevelStorage; }
	const LevelData* getLevelData() const { return &m_levelData; }
	AABBVector* getCubes(const Entity* pEnt, const AABB& aabb);
	std::vector<LightUpdate>* getLightsToUpdate();
	Player* getNearestPlayer(const Entity*, float) const;
	Player* getNearestPlayer(float x, float y, float z, float) const;

	// unused redstone stuff
	int getSignal(int x, int y, int z, int dir) const;
	int getDirectSignal(int x, int y, int z, int dir) const;
	bool hasDirectSignal(int x, int y, int z) const;
	bool hasNeighborSignal(int x, int y, int z) const;

#ifdef ENH_IMPROVED_SAVING
	void saveUnsavedChunks();
#endif

public:
	AABBVector m_aabbs;
	bool m_bInstantTicking;
	bool m_bIsMultiplayer; // if the level is controlled externally by a server
	bool m_bPostProcessing;
	EntityVector m_entities;
	std::vector<Player*> m_players;
	int m_skyDarken;
	uint8_t field_30;
	Dimension* m_pDimension;
	Random m_random;
	bool m_bCalculatingInitialSpawn;
	std::vector<LevelListener*> m_levelListeners;
	ChunkSource* m_pChunkSource;
	LevelStorage* m_pLevelStorage;
	int field_AA8;
	int field_AAC;
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

