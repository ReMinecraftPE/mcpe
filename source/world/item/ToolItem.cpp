#include "ToolItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

ToolItem::ToolItem(int id, Tool::Type toolType, Tier& tier) :
	Item(id),
	m_speed(tier.m_speed),
	m_toolType(toolType),
	m_tier(tier)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}


float ToolItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	if ((tile->m_toolMask & m_toolType) || (tile->m_pMaterial->m_toolMask & m_toolType))
		return m_speed;

	return 1.0f;
}

void ToolItem::hurtEnemy(ItemStack* instance, Mob* mob) const
{
	instance->hurtAndBreak(2, mob);
}

void ToolItem::mineBlock(ItemStack* instance, const TilePos& pos, Facing::Name face, Mob* mob) const
{
	instance->hurtAndBreak(1, mob);
}

int ToolItem::getAttackDamage(Entity* entity) const
{
	int damage = m_tier.m_damage + 1; // Just add 1 extra damage here since all tools did the same.
	/*
	if ((Tool::HATCHET & m_toolType))
		damage += 1;
	else if ((Tool::PICKAXE & m_toolType))
		damage += 1;
	else if ((Tool::SHOVEL & m_toolType))
		damage += 1;
	*/
	return damage;
}

bool ToolItem::isHandEquipped() const
{
	return true;
}

bool ToolItem::canDestroySpecial(const Tile* tile) const
{
	if (tile->m_toolMask & m_toolType)
		return m_tier.m_level >= tile->m_requiredToolLevel;

	if (tile->m_pMaterial->m_toolMask & m_toolType)
		return m_tier.m_level >= tile->m_pMaterial->m_requiredToolLevel;

	return false;
}