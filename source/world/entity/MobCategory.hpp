#pragma once
#include "EntityCategories.hpp"

class Material;

class MobCategory
{
public:
	enum ID {
		MONSTER,
		CREATURE,
		// WATER_CREATURE
	};


	static MobCategory monster;
	static MobCategory creature;
	static MobCategory waterCreature;
	static const MobCategory values[];
	static const int numValues;

private:
	MobCategory(const EntityCategories&, int, int, const Material*, bool);

public:
	static void initMobCategories();

public:
	const EntityCategories& getBaseType() const { return m_baseType; }
	int getMaxInstancesPerChunk() const { return m_maxInstancesPerChunk; }
	const Material* getSpawnPositionMaterial() const { return m_pSpawnPositionMaterial; }
	bool isFriendly() const { return m_bIsFriendly; }

	// custom addition
	static MobCategory& GetCategoryByIndex(MobCategory::ID i);

private:
	const EntityCategories m_baseType;
	int field_4;
	int m_maxInstancesPerChunk;
	const Material* m_pSpawnPositionMaterial;
	bool m_bIsFriendly;
};

