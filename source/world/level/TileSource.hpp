#pragma once
#include <memory>
#include <thread>
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/entity/EntityType.hpp"
#include "world/tile/Tile.hpp"
#include "world/level/Brightness.hpp"
#include "world/level/TileChange.hpp"
#include "client/renderer/LightLayer.hpp"
#include "world/entity/MobSpawnerData.hpp"

class TileTickingQueue;
class AABB;
class LevelChunk;
class TileEntity;
class TileSourceListener; // TODO
class TileTickingQueue;
class Dimension;
class ChunkSource;
struct Bounds;

class TileSource
{
protected:
	std::thread::id m_threadId;
	bool m_allowUnpopulatedChunks;
	bool m_publicSource;
	Level& m_level;
	ChunkSource& m_chunkSource;
	Dimension& m_dimension;
	std::vector<TileSourceListener*> m_tileSourceListeners;
	LevelChunk* m_lastChunk;
	TileTickingQueue* m_tileTickingQueue;

public:
	TileSource(Level& level, Dimension& dimension, ChunkSource& source, bool publicSource, bool allowUnpopulatedChunks);
	virtual ~TileSource();

public:
	Level& getLevel() const;
	const Level& getLevelConst() const;
	Dimension& getDimension() const;
	const Dimension& getDimensionConst() const;
	ChunkSource& getChunkSource()
	{
		return m_chunkSource;
	}
	void setTickingQueue(TileTickingQueue& queue);
	TileTickingQueue* getTickQueue(const TilePos& pos);
	LevelChunk* getChunk(const ChunkPos& pos);
	LevelChunk* getChunk(int x, int z);
	LevelChunk* getWritableChunk(const ChunkPos& pos);
	bool shouldFireEvents(LevelChunk& chunk) const;
	bool hasChunksAt(const Bounds& bounds);
	bool hasChunksAt(const AABB& aabb);
	bool hasChunksAt(const TilePos& min, const TilePos& max);
	bool hasChunksAt(const TilePos& pos, int r);
	bool hasChunksAt(int x, int y, int z, int r);
	Brightness_t getSkyDarken();
	const std::vector<MobSpawnerData>& getMobsAt(EntityType entityType, const TilePos& pos);
	bool hasNeighborSignal(const TilePos& pos); // custom
	bool hasNeighborSignal(int x, int y, int z);
	void runLightUpdates(const LightLayer&, const TilePos&, const TilePos&); // unk type
	bool isTopSolidBlocking(Tile*, uint8_t);
	void onChunkDiscarded(LevelChunk& chunk);
	void clearCachedLastChunk(); // unk type
	void fireAreaChanged(const TilePos&, const TilePos&); // unk type
	void fireTilesDirty(int, int, int, int, int, int); // unk type
	void fireTileChanged(const TilePos&, FullTile, FullTile, TileChange);
	void fireBrightnessChanged(const TilePos&); // unk type
	void fireTileEntityRemoved(std::unique_ptr<TileEntity>&); // unk type
	void fireTileEvent(int, int, int, int, int); // unk type
	void getChunkAt(const TilePos&); // unk type
	void hasTile(const TilePos&); // unk type
	void fireTileEntityChanged(TileEntity&); // unk type
	void getChunkAt(int, int, int); // unk type
	void hasTile(int, int, int); // unk type
	bool isOwnerThread() const;
	void getDimensionId(); // unk type
	void setExtraData(const TilePos&, uint16_t); // unk type
	void getExtraData(const TilePos&); // unk type
	TileID getTopSolidBlock(const TilePos&, bool);
	TileID getTopSolidBlock(int, int, bool);
	bool canSeeSky(const TilePos&);
	bool canSeeSky(int, int, int);
	uint8_t getHeightmap(const TilePos&);
	void getHeightmapPos(const TilePos&); // unk type
	uint8_t getHeightmap(int, int);
	void getTileEntity(const TilePos&); // unk type
	void getTileEntity(int, int, int); // unk type
	void getGrassColor(const TilePos&); // unk type
	void getTopRainTilePos(const TilePos&); // unk type
	void checkIsTopRainTilePos(const TilePos&); // unk type
	TileID getTile(const TilePos& pos);
	TileID getTile(int x, int y, int z);
	Tile* getTilePtr(const TilePos& pos);
	Tile* getTilePtr(int x, int y, int z);
	TileData getData(const TilePos& pos);
	TileData getData(int x, int y, int z);
	bool isSolidBlockingTile(int, int, int);
	bool isSolidBlockingTile(const TilePos&);
	void isSolidRenderTile(int, int, int); // unk type
	void isSolidRenderTile(const TilePos&); // unk type
	bool containsMaterial(const AABB&, const Material*);
	void tileEvent(int, int, int, int, int); // unk type
	void tileEvent(const TilePos&, int, int); // unk type
	bool containsAnyLiquid(const AABB&);
	bool containsFireTile(const AABB&);
	Material* getMaterial(const TilePos&);
	Material* getMaterial(int, int, int);
	void isWaterAt(const TilePos&); // unk type
	void isIceAt(const TilePos&); // unk type
	void updateNeighborsAt(const TilePos&, TileID); // unk type
	bool isEmptyTile(const TilePos&);
	bool isEmptyTile(int, int, int);
	void findNextTopSolidTileUnder(TilePos&); // unk type
	void containsAny(const TilePos&, const TilePos&); // unk type
	Brightness_t getRawBrightness(const TilePos& pos, bool unk = true);
	Brightness_t getRawBrightness(int x, int y, int z, bool unk = true);
	Brightness_t getBrightness(const LightLayer& lightLayer, const TilePos& pos);
	Brightness_t getBrightness(const LightLayer& lightLayer, int x, int y, int z);
	float getBrightness(const TilePos& pos);
	float getBrightness(int x, int y, int z);
	void updateLightIfOtherThan(const LightLayer&, const TilePos&, Brightness_t); // unk type
	void containsLiquid(const AABB&, const Material*); // unk type
	void getTileAndData(const TilePos&); // unk type
	void isTopSolidBlocking(const TilePos&); // unk type
	void isTopSolidBlocking(int, int, int); // unk type
	void getTileAndData(int, int, int); // unk type
	void clip(HitResult&, const Vec3&, const Vec3&, bool, bool);
	float getSeenPercent(const Vec3&, const AABB&);
	void getLightColor(const TilePos&, int); // unk type
	void getBiome(const TilePos&); // unk type
	void shouldFreeze(const TilePos&, bool); // unk type
	void shouldFreezeIgnoreNeighbors(const TilePos&); // unk type
	void shouldThaw(const TilePos&, bool); // unk type
	void isSnowTemperature(const TilePos&); // unk type
	void isHumidAt(const TilePos&); // unk type
	bool setTileAndData(const TilePos&, FullTile, TileChange updateFlags = TileChange::UPDATE_NONE);
	bool setTileAndData(int, int, int, FullTile, TileChange updateFlags = TileChange::UPDATE_NONE);
	bool setTile(const TilePos&, TileID, TileChange updateFlags = TileChange::UPDATE_NONE); // custom
	bool setTile(int, int, int, TileID, TileChange updateFlags = TileChange::UPDATE_NONE);
	bool setTileAndDataNoUpdate(const TilePos&, FullTile); // custom
	bool setTileAndDataNoUpdate(int, int, int, FullTile);
	bool setTileNoUpdate(const TilePos&, TileID); // custom
	bool setTileNoUpdate(int, int, int, TileID);
	void removeTile(int, int, int); // unk type
	void setBrightness(const LightLayer&, const TilePos&, Brightness_t); // unk type
	void setBrightness(const LightLayer&, int, int, int, Brightness_t); // unk type
	void setGrassColor(int, const TilePos&, int); // unk type
	void fetchCollisionShapes(Entity*, const AABB&, float*, bool); // unk type
	std::vector<AABB>& fetchAABBs(const AABB&, bool); // unk type
	void getEntities(EntityType, const AABB&, Entity*); // unk type
	void getNearestEntityOfType(Entity*, const AABB&, EntityType); // unk type
	void getNearestEntityOfType(Entity*, const Vec3&, float, EntityType); // unk type
	const std::vector<Entity*>& getEntities(Entity*, const AABB&); // unk type
	void isUnobstructedByEntities(const AABB&, Entity*); // unk type
	bool mayPlace(TileID, const TilePos&, int8_t, Mob*, bool, Entity*);

protected:
	void _neighborChanged(const TilePos&, const TilePos&, TileID);
	void _tileChanged(const TilePos& pos, FullTile oldTile, FullTile newTile, TileChange updateFlags);
};
