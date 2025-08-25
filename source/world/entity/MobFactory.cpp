#include "MobFactory.hpp"
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
             ENT(CREEPER, Creeper) \
             ENT(ZOMBIE, Zombie) \
             ENT(SPIDER, Spider) \
             ENT(SKELETON, Skeleton) \
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

Entity* MobFactory::LoadEntity(const CompoundTag& tag, Level* level)
{
    Entity* entity = nullptr;
    EntityType::ID entityTypeId = (EntityType::ID)tag.getInt32("id");
    
    if (entityTypeId == EntityType::ITEM)
    {
        entity = new ItemEntity(level);
    }
    else
    {
        entity = MobFactory::CreateMob(entityTypeId, level);
    }

    if (entity)
    {
        entity->load(tag);

        if (entity->getDescriptor().isType(EntityType::ITEM))
        {
            ItemInstance& itemInstance = ((ItemEntity*)entity)->m_itemInstance;
            if (!Item::items[itemInstance.m_itemID])
            {
                delete entity;
                entity = nullptr;
            }
        }
    }

    return entity;
}

#undef ENT