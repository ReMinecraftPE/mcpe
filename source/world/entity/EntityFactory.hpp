#pragma once

#include "EntityType.hpp"
#include "Entity.hpp"

class EntityFactory
{
public:
	static Entity* CreateEntity(EntityType::ID entityType, TileSource& source);
	static Entity* LoadEntity(const CompoundTag& tag, TileSource& source);
};
