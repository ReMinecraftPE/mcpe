#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobFactory
{
public:
	static Mob* CreateMob(EntityType::ID entityType, Level *level);
};
