#pragma once
#include "EntityCategories.hpp"

class Material;

class MobCategory
{
public:
	static MobCategory monster;
	static MobCategory creature;
	static MobCategory waterCreature;
	static const MobCategory* all[];
	static const unsigned int allCount;

private:
	MobCategory(const EntityCategories& baseType, int min, int max, const Material* material, bool friendly);

public:
	static void initMobCategories();

public:
	const EntityCategories& getBaseType() const { return m_baseType; }
	int getMaxInstancesPerChunk() const { return m_max; }
	const Material* getSpawnPositionMaterial() const { return m_pSpawnPositionMaterial; }
	bool isFriendly() const { return m_bIsFriendly; }

private:
	EntityCategories m_baseType;
	int m_min;
	int m_max;
	const Material* m_pSpawnPositionMaterial;
	bool m_bIsFriendly;
};

