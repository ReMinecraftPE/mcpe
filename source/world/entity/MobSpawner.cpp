#include "world/entity/MobSpawner.hpp"
#include "world/entity/MobFactory.hpp"
#include "world/level/TileSource.hpp"

#define MOB_SPAWNER_HOSTILE_BRIGHTNESS   7
#define MOB_SPAWNER_FRIENDLY_BRIGHTNESS  9

void MobSpawner::tick(TileSource& source, bool allowHostile, bool allowFriendly) 
{
    if (!allowHostile && !allowFriendly)
        return;

    chunksToPoll.clear();

    for (std::vector<Player*>::const_iterator it = source.getLevelConst().m_players.begin(); it != source.getLevelConst().m_players.end(); ++it)
    {
        const Player* player = *it;
        int cx = Mth::floor(player->m_pos.x / 16.0f);
        int cz = Mth::floor(player->m_pos.z / 16.0f);

        for (int dx = -8; dx <= 8; ++dx) 
        {
            for (int dz = -8; dz <= 8; ++dz) 
            {
                chunksToPoll.insert(ChunkPos(cx + dx, cz + dz));
            }
        }
    }

    Level& level = source.getLevel();

    for (unsigned int i = 0; i < MobCategory::allCount; i++)
    {
        const MobCategory& category = *MobCategory::all[i]; 
        const EntityCategories& baseType = category.getBaseType();
        bool isFriendly = category.isFriendly();

        // good mobs don't spawn after dark, otherwise they will crowd around torches like beta
        if (!source.getDimensionConst().isDay() && isFriendly)
            continue;

        if ((isFriendly && !allowFriendly) || (!isFriendly && !allowHostile))
            continue;

        // TODO
        //if (level.getEntityCount(baseType) > static_cast<unsigned int>(category.getMaxInstancesPerChunk() * static_cast<int>(chunksToPoll.size()) / 256))
        //    continue;

        for (std::set<ChunkPos>::iterator it = chunksToPoll.begin(); it != chunksToPoll.end(); ++it) 
        {
            const ChunkPos& pos = *it;

            const std::map<EntityType::ID, int>& spawnList = MobFactory::GetMobListOfCategory(baseType);
                                
            if (spawnList.empty())
                continue;

            EntityType::ID entityID = spawnList.begin()->first;

            int spawnWeight = 1; // make sure it starts with 1 so arithmetic exception doesn't occur

            for (std::map<EntityType::ID, int>::const_iterator it = spawnList.begin(); it != spawnList.end(); ++it)
                spawnWeight += it->second;

            int randomRate = level.m_random.nextInt(spawnWeight);

            for (std::map<EntityType::ID, int>::const_iterator it = spawnList.begin(); it != spawnList.end(); ++it)
            {
                randomRate -= it->second;
                if (randomRate < 0) 
                {
                    entityID = it->first;
                    break;
                }
            }

            TilePos tpos = getRandomPosWithin(source, pos.x * 16, pos.z * 16);

            if (source.isSolidBlockingTile(tpos) || source.getMaterial(tpos) != category.getSpawnPositionMaterial()) 
                continue;

            int spawned = 0;
            for (int i = 0; i < 3; ++i)
            {
                TilePos tp(tpos);

                if (spawned == -1)
                    break;

                for (int j = 0; j < 4; ++j) 
                {
                    tp.x += level.m_random.nextInt(6) - level.m_random.nextInt(6);
                    tp.y += level.m_random.nextInt(1) - level.m_random.nextInt(1);
                    tp.z += level.m_random.nextInt(6) - level.m_random.nextInt(6);

                    if (!IsSpawnPositionOk(category, source, tp)) 
                        continue;

                    Brightness_t lightLevel = source.getRawBrightness(tp);
                    if (isFriendly)
                    {
                        if (lightLevel < MOB_SPAWNER_FRIENDLY_BRIGHTNESS)
                            continue;
                    }
                    else
                    {
                        if (lightLevel > MOB_SPAWNER_HOSTILE_BRIGHTNESS)
                            continue;
                    }

                    Vec3 pPos(tp.x + 0.5, tp.y, tp.z + 0.5);

                    if (level.getNearestPlayer(pPos, 24.0f, false)) 
                        continue;
                    
                    Vec3 dPos = pPos - level.getSharedSpawnPos();
                    
                    if (dPos.lengthSqr() < 576.0f) 
                        continue;
                    
                    Mob* entity = MobFactory::CreateMob(entityID, source);
                    if (!entity) 
                        break;

                    entity->moveTo(pPos, Vec2(level.m_random.nextFloat() * 360.0f, 0.0f));
                    
                    if (entity->canSpawn()) 
                    {
                        ++spawned;
                        level.addEntity(entity);
                        FinalizeMobSettings(entity, source, pPos);
                        if (spawned >= entity->getMaxSpawnClusterSize())
                        {
                            spawned = -1;
                            break;
                        }
                    }
                }
            }
        }
    }
}

TilePos MobSpawner::getRandomPosWithin(TileSource& source, int chunkX, int chunkZ)
{
    int px = source.getLevel().m_random.nextInt(16) + chunkX;
    int py = source.getLevel().m_random.nextInt(128);
    int pz = source.getLevel().m_random.nextInt(16) + chunkZ;
    return TilePos(px, py, pz);
}

bool MobSpawner::AddMob(TileSource& source, Mob *mob, const Vec3& pos, const Vec2& rot)
{
    if (!mob)
        return false;

    if (!mob->canSpawn() || !mob->isAlive())
        return false;

    mob->moveTo(pos, rot);
    source.getLevel().addEntity(mob);
    FinalizeMobSettings(mob, source, pos);

    return true;
}

bool MobSpawner::IsSpawnPositionOk(const MobCategory& category, TileSource& source, const TilePos& pos) 
{
    if (category.getSpawnPositionMaterial() == Material::water) 
        return source.getMaterial(pos)->isLiquid() && !source.isSolidBlockingTile(pos.above());

    return source.isSolidBlockingTile(pos.below()) && !source.isSolidBlockingTile(pos) && !source.getMaterial(pos)->isLiquid() && !source.isSolidBlockingTile(pos.above());
}

void MobSpawner::FinalizeMobSettings(Mob *mob, TileSource& source, const Vec3& pos)
{
    if (!mob)
        return;

    //mob->finalizeMobSpawn();
    MakeBabyMob(mob, source);
}


void MobSpawner::MakeBabyMob(Mob *mob, TileSource& source)
{
    source.getLevel().m_random.setSeed(0x5deea8f);

    if (mob->isBaby())
        return;

    // 0.6.0+
}


void MobSpawner::PostProcessSpawnMobs(TileSource& source, Biome& biome, const Vec3& pos)
{
  // empty (0.7.1)
}