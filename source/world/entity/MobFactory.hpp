#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobFactory
{
public:
	static Mob* CreateMob(EntityType entityType, Level *level);
	static std::string GetMobNameID(EntityType entityType);
	static EntityType GetEntityTypeFromMobName(std::string mobName);
};