#pragma once

#include "EntityType.hpp"
#include "Mob.hpp"

class MobCategory;

class MobFactory
{
public: 
    static void initMobLists();
	static Mob* CreateMob(EntityType::ID entityType, Level *level);
	static std::map<EntityType::ID, int> GetMobListOfCategory(EntityCategories::CategoriesMask category);
};