#include "ToolItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

ToolItem::Tier
	ToolItem::Tier::WOOD   	(0, 59,   2.0f,  0),
	ToolItem::Tier::STONE  	(1, 131,  4.0f,  1),
	ToolItem::Tier::IRON   	(2, 250,  6.0f,  2),
	ToolItem::Tier::EMERALD	(3, 1561, 8.0f,  3),
	ToolItem::Tier::GOLD   	(0, 32,   12.0f, 0);

ToolItem::ToolItem(int id, Tool::Type type, Tier& tier) :
	Item(id),
	m_speed(tier.m_speed),
	m_tier(tier),
	m_toolType(type)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

bool ToolItem::_canDestroyTile(const Tile* tile) const
{
	return (tile->m_toolMask & m_toolType) != 0;
}

bool ToolItem::_canDestroyMaterial(const Material* material) const
{
	return (material->m_toolMask & m_toolType) != 0;
}

float ToolItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	return (_canDestroyTile(tile) || _canDestroyMaterial(tile->m_pMaterial)) ? m_speed : 1.0f;
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
	if (_canDestroyTile(tile))
		return m_tier.m_level >= tile->m_requiredToolLevel;

	if (_canDestroyMaterial(tile->m_pMaterial))
		return m_tier.m_level >= tile->m_pMaterial->m_requiredToolLevel;

	return false;
}