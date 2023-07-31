/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>

#include "Tile.hpp"
#include "Entity.hpp"
#include "LocalPlayer.hpp"
#include "Dimension.hpp"
#include "LevelChunk.hpp"
#include "ChunkSource.hpp"
#include "LevelStorageSource.hpp"
#include "LevelSource.hpp"
#include "LevelData.hpp"
#include "LevelListener.hpp"
#include "LightUpdate.hpp"
#include "TickNextTickData.hpp"

class Dimension;
class Level;
class LevelListener;

typedef std::vector<Entity*> EntityVector;
typedef std::vector<AABB> AABBVector;

class Level : public LevelSource
{
public:
	Level(LevelStorage*, const std::string&, TLong seed, int);
	Level(LevelStorage*, const std::string&, TLong seed, int, Dimension*);
	~Level();

	// TODO
	TileID getTile(int x, int y, int z) override;
	float getBrightness(int x, int y, int z) override;
	int getData(int x, int y, int z) override;
	Material* getMaterial(int x, int y, int z) override;
	bool isSolidTile(int x, int y, int z) override;

	void _init(const std::string& str, TLong  seed, int x, Dimension* pDimension);
	ChunkSource* getChunkSource();
	ChunkSource* createChunkSource();
	LevelChunk* getChunk(int x, int z);
	LevelChunk* getChunkAt(int x, int z);
	int getRawBrightness(int x, int y, int z);
	int getRawBrightness(int x, int y, int z, bool b);
	int getBrightness(const LightLayer&, int x, int y, int z);
	void setBrightness(const LightLayer&, int x, int y, int z, int bright);
	int getSeaLevel();
	int getSeed();
	TLong getTime();
	int getHeightmap(int x, int z);
	bool isDay();
	bool isSkyLit(int x, int y, int z);
	bool isEmptyTile(int x, int y, int z);
	bool hasChunkAt(int x, int y, int z);
	bool hasChunk(int x, int z);
	bool hasChunksAt(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
	bool hasChunksAt(int x, int y, int z, int rad);
	void updateSkyBrightness();
	float getTimeOfDay(float f);
	int getSkyDarken(float f);
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
	Pos getSharedSpawnPos();
	void validateSpawn();
	TileID getTopTile(int x, int z);
	int getTopTileY(int x, int z);
	int getTopSolidBlock(int x, int z);
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
	void setSpawnPos(Pos);
	void setSpawnSettings(bool, bool);
	bool canSeeSky(int x, int y, int z);
	Vec3 getSkyColor(Entity* pEnt, float f);
	Vec3 getFogColor(float f);
	bool isUnobstructed(AABB*);
	bool mayInteract(Player* player, int x, int y, int z);
	bool mayPlace(TileID tid, int x, int y, int z, bool b);
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
	void playSound(Entity*, const std::string& name, float, float);
	void playSound(float x, float y, float z, const std::string& name, float, float);
	void animateTick(int x, int y, int z);
	float getSeenPercent(Vec3, AABB);
	void explode(Entity*, float x, float y, float z, float power);
	void explode(Entity*, float x, float y, float z, float power, bool bIsFiery);
	void addEntities(const std::vector<Entity*>& entities);
	void ensureAdded(Entity* entity);
	void extinguishFire(int x, int y, int z, int dir);
	int  findPath(Entity* ent1, Entity* ent2, float f);
	int  findPath(Entity* ent1, int x, int y, int z, float f);
	int  getLightDepth(int x, int z);
	float getStarBrightness(float f);
	float getSunAngle(float f);
	void setTime(TLong time);
	void swap(int x1, int y1, int z1, int x2, int y2, int z2);

	HitResult clip(const Vec3& a, const Vec3& b);
	HitResult clip(Vec3 a, Vec3 b, bool c);
	Entity* getEntity(int id);
	EntityVector* getAllEntities();
	EntityVector* getEntities(Entity* pAvoid, const AABB&);
	BiomeSource* getBiomeSource();
	LevelStorage* getLevelStorage();
	LevelData* getLevelData();
	AABBVector* getCubes(const Entity* pEnt, const AABB& aabb);
	std::vector<LightUpdate>* getLightsToUpdate();
	Entity* getNearestPlayer(Entity*, float);
	Entity* getNearestPlayer(float x, float y, float z, float);

	// unused redstone stuff
	int getSignal(int x, int y, int z, int dir);
	int getDirectSignal(int x, int y, int z, int dir);
	bool hasDirectSignal(int x, int y, int z);
	bool hasNeighborSignal(int x, int y, int z);

public:
	AABBVector m_aabbs;
	uint8_t field_10 = 0;
	uint8_t field_11 = 0;
	uint8_t field_12 = 0;
	EntityVector m_entities;
	std::vector<Player*> m_players;
	int m_skyDarken = 0;
	uint8_t field_30 = 0;
	Dimension* m_pDimension = nullptr;
	Random field_38;
	uint8_t field_A00 = 0;
	std::vector<LevelListener*> m_levelListeners;
	ChunkSource* m_pChunkSource = nullptr;
	LevelStorage* m_pLevelStorage = nullptr;
	LevelData m_levelData;
	int field_AA8 = 42184323;
	int field_AAC = 1013904223;
	EntityVector m_getEntitiesResult;
	EntityVector m_pendingEntityRemovals;
	std::set<TickNextTickData> m_pendingTicks;
	std::set<ChunkPos> m_chunksToUpdate;
	std::vector<LightUpdate> m_lightUpdates;
	bool m_bUpdateLights = true;
	int field_B08 = 0;
	uint8_t field_B0C = 0;
	int field_B10;
};

