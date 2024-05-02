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

#define ENTS ENT(CHICKEN, Chicken) \
             ENT(COW, Cow) \
             ENT(PIG, Pig) \
             //ENT(SHEEP, Sheep) \
             //ENT(ZOMBIE, Zombie) \
             //ENT(CREEPER, Creeper) \
             //ENT(SKELETON, Skeleton) \
             //ENT(SPIDER, Spider) \
             //ENT(PIG_ZOMBIE, PigZombie)

#define ENT(enumType, classType) case ENTITY_TYPE_ ## enumType: return new classType(level);

Mob* MobFactory::CreateMob(EntityType entityType, Level *level)
{
    switch (entityType)
    {
        ENTS;
    default:
        LOG_W("Unknown mob type requested: %d", entityType);
        return nullptr;
    }
}

#undef ENT

#define ENT(enumType, classType) case ENTITY_TYPE_ ## enumType: return "entity."###classType;

char* MobFactory::GetMobNameID(EntityType entityType)
{
    switch (entityType)
    {
        ENTS;
    default:
        return "";
    }
}

#undef ENT

#define ENT(enumType, classType) if (mobName == #classType) return ENTITY_TYPE_ ## enumType;

EntityType MobFactory::GetEntityTypeFromMobName(std::string mobName)
{
    ENTS;

    return ENTITY_TYPE_NONE;
}

#undef ENT