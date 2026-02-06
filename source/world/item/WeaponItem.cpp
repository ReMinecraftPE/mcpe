#include "WeaponItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

WeaponItem::WeaponItem(int id, Tier& tier) : ToolItem(id, Tool::SWORD, tier), m_attackDamage(4 + tier.m_damage)
{
}

float WeaponItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	return (_canDestroyTile(tile) || _canDestroyMaterial(tile->m_pMaterial)) ? 15.0f : 1.5f;
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