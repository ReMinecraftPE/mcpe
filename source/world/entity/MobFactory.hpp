#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobCategory;

class MobFactory
{
public:
	static void initMobLists();
	static Mob* CreateMob(EntityType::ID entityType, TileSource& source);
	static const std::map<EntityType::ID, int>& GetMobListOfCategory(const EntityCategories& category);
};
