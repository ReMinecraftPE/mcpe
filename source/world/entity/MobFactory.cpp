#include "MobFactory.hpp"
#include "Chicken.hpp"
#include "Cow.hpp"
#include "Pig.hpp"
//#include "Sheep.hpp"
//#include "Zombie.hpp"
#include "Creeper.hpp"
//#include "Skeleton.hpp"
//#include "Spider.hpp"
//#include "PigZombie.hpp"

Mob* MobFactory::CreateMob(EntityType entityType, Level *level)
{
    switch (entityType)
    {
    case ENTITY_TYPE_CHICKEN: return new Chicken(level);
    //case ENTITY_TYPE_COW: return new Cow(level);
    case ENTITY_TYPE_PIG: return new Pig(level);
    //case ENTITY_TYPE_SHEEP: return new Sheep(level);
    //case ENTITY_TYPE_ZOMBIE: return new Zombie(level);
    //case ENTITY_TYPE_CREEPER: return new Creeper(level);
    //case ENTITY_TYPE_SKELETON: return new Skeleton(level);
    //case ENTITY_TYPE_SPIDER: return new Spider(level);
    //case ENTITY_TYPE_PIG_ZOMBIE: return new PigZombie(level);
    default:
        LOG_W("Unknown mob type requested: %d", entityType);
        return nullptr;
    }
}

char* MobFactory::GetMobNameID(EntityType entityType)
{
    switch (entityType)
    {
    case ENTITY_TYPE_CHICKEN: return "entity.Chicken";
    case ENTITY_TYPE_COW: return "entity.Cow";
    case ENTITY_TYPE_PIG: return "entity.Pig";
    case ENTITY_TYPE_SHEEP: return "entity.Sheep";
    case ENTITY_TYPE_ZOMBIE: return "entity.Zombie";
    case ENTITY_TYPE_CREEPER: return "entity.Creeper";
    case ENTITY_TYPE_SKELETON: return "entity.Skeleton";
    case ENTITY_TYPE_SPIDER: return "entity.Spider";
    case ENTITY_TYPE_PIG_ZOMBIE: return "entity.PigZombie";
    default:
        return "";
    }
}