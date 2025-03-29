#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobFactory
{
public:
	static Entity* CreateMob(EntityType::ID entityType, Level *level);
};