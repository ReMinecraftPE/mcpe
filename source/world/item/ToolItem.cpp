#include "ToolItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

ToolItem::ToolItem(int id, int attackDamageBase, Tier& tier) : Item(id), m_speed(tier.m_speed), m_attackDamage(attackDamageBase + tier.m_damage), m_tier(tier)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

void ToolItem::initializeTiles(const Tile** tiles, int tileCount)
{
	// Up for alternate solutions. The variables are initialized from an array during runtime to remove nondeterministic static array initialization time.
	// Since the tiles do not have an opportunity to initialize with a statically allocated array and initializer lists are out of the question, this was my compromise.
	// See PickaxeItem for use of impl
	m_tiles.reserve(tileCount);
	for (int i = 0; i < tileCount; ++i)
        m_tiles.push_back(tiles[i]);
}


float ToolItem::getDestroySpeed(ItemStack* instance, const Tile* tile) const
{
	if (std::find(m_tiles.begin(), m_tiles.end(), tile) != m_tiles.end())
	{
		return m_speed;
	}
	return 1.0f;
}

void ToolItem::hurtEnemy(ItemStack* instance, Mob* mob) const
{
	instance->hurtAndBreak(2, mob);
}

void ToolItem::mineBlock(ItemStack* instance, const TilePos& pos, Facing::Name face, Player* player) const
{
	instance->hurtAndBreak(1, player);
}

int ToolItem::getAttackDamage(Entity* entity) const
{
	return m_attackDamage;
}

bool ToolItem::isHandEquipped() const
{
	return true;
}