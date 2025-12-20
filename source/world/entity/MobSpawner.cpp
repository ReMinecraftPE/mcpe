
#include "world/entity/MobSpawner.hpp"
#include "world/entity/MobFactory.hpp"

#define MOB_SPAWNER_HOSTILE_BRIGHTNESS   7

void MobSpawner::tick(Level *level, bool allowHostile, bool allowFriendly) 
{
    if (!allowHostile && !allowFriendly)
        return;

    chunksToPoll.clear();

    for (std::vector<Player*>::const_iterator it = level->m_players.begin(); it != level->m_players.end(); ++it)
    {
        Player* player = *it;
        int cx = Mth::floor(player->m_pos.x / 16.0f);
        int cz = Mth::floor(player->m_pos.z / 16.0f);

        for (int dx = -8; dx <= 8; ++dx) {
            for (int dz = -8; dz <= 8; ++dz) {
                chunksToPoll.insert(ChunkPos(cx + dx, cz + dz));
            }
        }
    }

    int totalSpawned = 0;

    for (int i = 0; i < MobCategory::numValues; i++)
    {
        /*
            Note:
            MobCategory::values does not work, we may want to refactor it so it initializes during static initialization
            pCategory->getBaseTyoe() returns bad data, hence a seeparate CategoriesMask is used

            i have no idea why above happens, blame c++!
        */
        MobCategory* pCategory = nullptr; 
        EntityCategories::CategoriesMask mask;
        switch (i) {
            case 0:
                pCategory = &MobCategory::monster;
                mask = EntityCategories::MONSTER;
                break;
            case 1:
                pCategory = &MobCategory::creature;
                mask = EntityCategories::ANIMAL;
                break;
            default:
                continue;
        }

        if ((level->getTime() % 12000 - 12000) >= 0 && pCategory->isFriendly())
            continue;

        if ((!pCategory->isFriendly() || allowFriendly) && (pCategory->isFriendly() || allowHostile) 
            && level->getEntityCountOfCategory(mask) <= (pCategory->getMaxInstancesPerChunk() * (int)chunksToPoll.size() / 256))
        {    
            for (std::set<ChunkPos>::iterator it = chunksToPoll.begin(); it != chunksToPoll.end(); ++it) {
                ChunkPos pos = *it;

                std::map<EntityType::ID, int> spawnList = MobFactory::GetMobListOfCategory(mask);
               
                assert(!spawnList.empty());
                    
                if (spawnList.empty()) 
                    continue;

                EntityType::ID type = spawnList.begin()->first;

                int spawnWeight = 1; // make sure it starts with 1 so arithmetic exception doesn't occur    

                for (std::map<EntityType::ID, int>::iterator it = spawnList.begin(); it != spawnList.end(); ++it)
                {
                    spawnWeight += it->second;
                }

                int randomRate = level->m_random.nextInt(spawnWeight);

                for (std::map<EntityType::ID, int>::iterator it = spawnList.begin(); it != spawnList.end(); ++it)
                {
                    randomRate -= it->second;
                    if (randomRate < 0) {
                        type = it->first;
                        break;
                    }
                }

                int idx = level->m_random.nextInt((int)spawnList.size());
                TilePos tpos = getRandomPosWithin(level, pos.x * 16, pos.z * 16);

                if (level->isSolidTile(tpos) || level->getMaterial(tpos) != pCategory->getSpawnPositionMaterial()) 
                    continue;

                int spawned = 0;
                for (int i = 0; i < 3; ++i)
                {
                    TilePos tp(tpos);

                    if (spawned == -1)
                        break;

                    for (int j = 0; j < 4; ++j) {
                        tp.x += level->m_random.nextInt(6) - level->m_random.nextInt(6);
                        tp.y += level->m_random.nextInt(1) - level->m_random.nextInt(1);
                        tp.z += level->m_random.nextInt(6) - level->m_random.nextInt(6);

                        if (IsSpawnPositionOk(pCategory, level, tp)) {
                            Vec3 pPos(tp.x + 0.5, tp.y, tp.z + 0.5);

                            if (!level->getNearestPlayer(pPos, 24.0f, false)) {
                                Vec3 dPos = pPos - level->getSharedSpawnPos();
                                if (dPos.lengthSqr() >= 576.0f) {

                                    
                                    Mob* entity = MobFactory::CreateMob(type, level);
                                    if (!entity) break;

                                    entity->moveTo(pPos, Vec2(level->m_random.nextFloat() * 360.0f, 0.0f));
                                    if (entity->canSpawn()) {
                                        ++spawned;
                                        level->addEntity(entity);
                                        FinalizeMobSettings(entity, level, pPos);
                                        if (spawned >= entity->getMaxSpawnClusterSize())
                                        {
                                            totalSpawned += spawned;
                                            spawned = -1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (spawned != -1)
                    totalSpawned += spawned;
            }
        }
    }

    return; // totalSpawned;
}

TilePos MobSpawner::getRandomPosWithin(Level *level, int chunkX, int chunkZ) 
{
    int px = chunkX + level->m_random.nextInt(16);
    int py = level->m_random.nextInt(128);
    int pz = chunkZ + level->m_random.nextInt(16);
    return TilePos(px, py, pz);
}

//todo: bool?
int MobSpawner::AddMob(Level *level, Mob *mob, const Vec3& pos, const Vec2& rot) 
{
    if (!level || !mob)
        return 0;

    if (!mob->canSpawn() || !mob->isAlive())
        return 0;

    mob->moveTo(pos, rot);
    level->addEntity(mob);
    FinalizeMobSettings(mob, level, pos);

    return 1;
}

bool MobSpawner::IsSpawnPositionOk(MobCategory *category, Level *level, const TilePos& pos) 
{
    int brightness = level->getRawBrightness(pos);

    if (!level->isEmptyTile(pos)) 
        return false;

    if (!category->isFriendly() && brightness >= MOB_SPAWNER_HOSTILE_BRIGHTNESS)
        return false;

    if (category->isFriendly() && brightness < MOB_SPAWNER_HOSTILE_BRIGHTNESS)
        return false;


    if (category->getSpawnPositionMaterial() == Material::water) 
        return level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());

    return level->isSolidTile(pos.below()) && !level->isSolidTile(pos) && !level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());
}

void MobSpawner::FinalizeMobSettings(Mob *mob, Level *level, const Vec3& pos) 
{
    if (!level || !mob)
        return;

    //mob->finalizeMobSpawn();
    MakeBabyMob(mob, level);
}


void MobSpawner::MakeBabyMob(Mob *mob, Level *level) 
{
    level->m_random.setSeed(0x5deea8f);

    if (mob->isBaby())
        return;

    // todo
}


void MobSpawner::PostProcessSpawnMobs(Level *level, Biome *biome, const Vec3& pos) 
{

}