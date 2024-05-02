#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobFactory
{
	static Mob* CreateMob(EntityType entityType, Level *level);
	static char* GetMobNameID(EntityType entityType);
	static EntityType MobFactory::GetEntityTypeFromMobName(std::string mobName);
};