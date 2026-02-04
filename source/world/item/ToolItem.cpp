#include "ToolItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

ToolItem::ToolItem(int id, Tool::Type toolType, Tier& tier) :
	Item(id),
	m_speed(tier.m_speed),
	m_tier(tier),
	m_toolType(toolType)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

float ToolItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	return (canDestroyTile(tile) || canDestroyMaterial(tile->m_pMaterial)) ? m_speed : 1.0f;
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
	return m_tier.m_damage + 1;
}

bool ToolItem::isHandEquipped() const
{
	return true;
}

bool ToolItem::canDestroySpecial(const Tile* tile) const
{
	if (canDestroyTile(tile))
		return m_tier.m_level >= tile->m_requiredToolLevel;

	if (canDestroyMaterial(tile->m_pMaterial))
		return m_tier.m_level >= tile->m_pMaterial->m_requiredToolLevel;

	return false;
}

bool ToolItem::canDestroyTile(const Tile* tile) const
{
	return tile->m_toolMask & m_toolType;
}

bool ToolItem::canDestroyMaterial(const Material* material) const
{
	return material->m_toolMask & m_toolType;
}