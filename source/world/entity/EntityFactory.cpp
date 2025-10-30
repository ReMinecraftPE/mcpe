#include "EntityFactory.hpp"
#include "MobFactory.hpp"
#include "common/Logger.hpp"
#include "nbt/CompoundTag.hpp"

#include "ItemEntity.hpp"
#include "PrimedTnt.hpp"
#include "FallingTile.hpp"
#include "Arrow.hpp"
//#include "Snowball.hpp"
//#include "ThrownEgg.hpp"
//#include "Painting.hpp"

#define ENTS ENT(ITEM, ItemEntity) \
             ENT(PRIMED_TNT, PrimedTnt) \
             ENT(FALLING_TILE, FallingTile) \
             ENT(ARROW, Arrow)
             //ENT(SNOWBALL, Snowball)
             //ENT(THROWN_EGG, ThrownEgg)
             //ENT(PAINTING, Painting)

#define ENT(enumType, classType) case EntityType::enumType: return new classType(level);

Entity* EntityFactory::CreateEntity(EntityType::ID entityType, Level* level)
{
    switch (entityType)
    {
        ENTS;
    default:
        LOG_W("Unknown entity type requested: %d", entityType);
        return nullptr;
    }
}

Entity* EntityFactory::LoadEntity(const CompoundTag& tag, Level* level)
{
    EntityType::ID entityTypeId = (EntityType::ID)tag.getInt32("id");
    if (entityTypeId < 0)
    {
        LOG_E("Negative ItemId: %d at EntityFactory::LoadEntity", entityTypeId);
        return nullptr;
    }

    const EntityTypeDescriptor* entityTypeDescriptor = EntityTypeDescriptor::GetByEntityTypeID(entityTypeId);
    if (!entityTypeDescriptor)
    {
        LOG_E("No EntityTypeDescriptor found for Entity ID: %d in EntityFactory::LoadEntity", entityTypeId);
        return nullptr;
    }

    Entity* entity = nullptr;

    if (entityTypeDescriptor->hasCategory(EntityCategories::MOB))
    {
        entity = MobFactory::CreateMob(entityTypeId, level);
    }
    else
    {
        entity = EntityFactory::CreateEntity(entityTypeId, level);
    }

    if (entity)
    {
        entity->load(tag);

        if (entityTypeDescriptor->isType(EntityType::ITEM))
        {
            ItemInstance* itemInstance = ((ItemEntity*)entity)->m_pItemInstance;
            if (ItemInstance::isNull(itemInstance))
            {
                delete entity;
                entity = nullptr;
            }
        }
    }

    return entity;
}

#undef ENT