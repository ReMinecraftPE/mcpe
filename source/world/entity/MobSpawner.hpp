#pragma once

#include <set>

#include "world/tile/Tile.hpp"
#include "world/entity/Entity.hpp"
#include "world/entity/MobCategory.hpp"
#include "world/level/Level.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"

class Biome;

class MobSpawner {
private:
    static bool IsSpawnPositionOk(const MobCategory& category, TileSource& source, const TilePos& pos);
    static void FinalizeMobSettings(Mob* mob, TileSource& source, const Vec3& pos);
    static void MakeBabyMob(Mob* mob, TileSource& source);
    static void PostProcessSpawnMobs(TileSource& source, Biome& biome, const Vec3& pos);
    static bool AddMob(TileSource& source, Mob* mob, const Vec3& pos, const Vec2& rot = Vec2::ZERO);

public:
    TilePos getRandomPosWithin(TileSource& source, int chunkX, int chunkZ);
    void tick(TileSource& source, bool allowHostile, bool allowFriendly);
private:

    std::set<ChunkPos> chunksToPoll;
};