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

#undef ENT