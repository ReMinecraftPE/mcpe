#include "WeaponItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

WeaponItem::WeaponItem(int id, Tier& tier) : Item(id), m_attackDamage(4 + tier.m_damage), m_tier(tier)
{
    handEquipped();
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

float WeaponItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	return (tile->m_ID == Tile::web->m_ID) ? 15.0f : 1.5f;
}

void WeaponItem::hurtEnemy(ItemStack* instance, Mob* mob) const
{
	instance->hurtAndBreak(1, mob);
}

void WeaponItem::mineBlock(ItemStack* instance, const TilePos& pos, Facing::Name face, Mob* mob) const
{
	instance->hurtAndBreak(2, mob);
}

int WeaponItem::getAttackDamage(Entity* entity) const
{
	return m_attackDamage;
}