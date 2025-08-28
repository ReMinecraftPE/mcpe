#pragma once

#include "EntityType.hpp"
#include "Entity.hpp"

class EntityFactory
{
public:
	static Entity* CreateEntity(EntityType::ID entityType, Level* level);
	static Entity* LoadEntity(const CompoundTag& tag, Level* level);
};