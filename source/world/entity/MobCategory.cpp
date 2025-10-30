#include "MobCategory.hpp"
#include "compat/LegacyCPP.hpp"
#include "world/level/Material.hpp"

MobCategory MobCategory::monster       = MobCategory(EntityCategories(EntityCategories::MONSTER),      10, 20, nullptr, false);
MobCategory MobCategory::creature      = MobCategory(EntityCategories(EntityCategories::ANIMAL),       10, 15, nullptr, true);
MobCategory MobCategory::waterCreature = MobCategory(EntityCategories(EntityCategories::WATER_ANIMAL), 5,  10, nullptr, true);
const MobCategory MobCategory::values[] = {
	MobCategory::monster,
	MobCategory::creature,
	MobCategory::waterCreature
};
const int MobCategory::numValues = sizeof(MobCategory::values) / sizeof(MobCategory);

MobCategory::MobCategory(const EntityCategories& baseType, int unknown, int max, const Material* material, bool friendly)
	: m_baseType(baseType)
{
	m_maxInstancesPerChunk = max;
	field_4 = unknown;
	m_pSpawnPositionMaterial = material;
	m_bIsFriendly = friendly;
}

void MobCategory::initMobCategories()
{
	MobCategory::monster.m_pSpawnPositionMaterial       = Material::air;
	MobCategory::creature.m_pSpawnPositionMaterial      = Material::air;
	MobCategory::waterCreature.m_pSpawnPositionMaterial = Material::water;
}