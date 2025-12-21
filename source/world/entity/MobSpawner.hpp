#pragma once

#include <set>

#include "world/tile/Tile.hpp"
#include "world/entity/Entity.hpp"
#include "world/entity/MobCategory.hpp"
#include "world/level/Level.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"

class MobSpawner {
public:
    static bool IsSpawnPositionOk(MobCategory& category, Level& level, const TilePos& pos);
    static void FinalizeMobSettings(Mob* mob, Level& level, const Vec3& pos);
    static void MakeBabyMob(Mob* mob, Level& level);
    static void PostProcessSpawnMobs(Level& level, Biome& biome, const Vec3& pos);
    static int AddMob(Level& level, Mob* mob, const Vec3& pos, const Vec2& rot = Vec2::ZERO);

    TilePos getRandomPosWithin(Level& level, int chunkX, int chunkZ);
    void tick(Level& level, bool allowHostile, bool allowFriendly);
private:

    std::set<ChunkPos> chunksToPoll;
};