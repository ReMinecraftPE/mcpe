#include "MobFactory.hpp"
#include "common/Logger.hpp"
#include "nbt/CompoundTag.hpp"

#include "Chicken.hpp"
#include "Cow.hpp"
#include "Pig.hpp"
#include "Sheep.hpp"
#include "Zombie.hpp"
#include "Creeper.hpp"
#include "Skeleton.hpp"
#include "Spider.hpp"
//#include "PigZombie.hpp"

#define ENTS ENT(CHICKEN, Chicken) \
             ENT(COW, Cow) \
             ENT(PIG, Pig) \
             ENT(SHEEP, Sheep) \
             ENT(ZOMBIE, Zombie) \
             ENT(CREEPER, Creeper) \
             ENT(SKELETON, Skeleton) \
             ENT(SPIDER, Spider) \
             //ENT(PIG_ZOMBIE, PigZombie)

#define ENT(enumType, classType) case EntityType::enumType: return new classType(level);

// format: ID, spawnrate
std::map<EntityType::ID, int> monsterList;
std::map<EntityType::ID, int> creatureList;
std::map<EntityType::ID, int> waterCreatureList;
std::map<EntityType::ID, int> nullCreatureList;

Mob* MobFactory::CreateMob(EntityType::ID entityType, Level *level)
{
    switch (entityType)
    {
        ENTS;
    default:
        LOG_W("Unknown mob type requested: %d", entityType);
        return nullptr;
    }
}

void MobFactory::initMobLists() 
{
    // format: ID, spawnrate
    monsterList.insert(std::make_pair(EntityType::SPIDER,   10));
    monsterList.insert(std::make_pair(EntityType::ZOMBIE,   10));
    monsterList.insert(std::make_pair(EntityType::SKELETON, 10));
    monsterList.insert(std::make_pair(EntityType::CREEPER,  10));
    //monsterList.insert(std::make_pair(EntityType::SLIME,    10));

    creatureList.insert(std::make_pair(EntityType::SHEEP,   12));
    creatureList.insert(std::make_pair(EntityType::PIG,     10));
    creatureList.insert(std::make_pair(EntityType::CHICKEN, 10));
    creatureList.insert(std::make_pair(EntityType::COW,      8));
	
    waterCreatureList.insert(std::make_pair(EntityType::SQUID, 10));
}

const std::map<EntityType::ID, int>& MobFactory::GetMobListOfCategory(const EntityCategories& category) 
{
    if (category.contains(EntityCategories::MONSTER))
        return monsterList;
    if (category.contains(EntityCategories::ANIMAL))
        return creatureList;
    return nullCreatureList;
}

#undef ENT