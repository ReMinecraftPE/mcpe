#pragma once

#include "Item.hpp"

class WeaponItem : public Item
{
public:
	WeaponItem(int id, Tier& tier);

	float getDestroySpeed(ItemStack* instance, const Tile* tile) const override;
	void hurtEnemy(ItemStack* instance, Mob* mob) const override;
	void mineBlock(ItemStack* instance, const TilePos& pos, Facing::Name face, Mob* mob) const override;
	int getAttackDamage(Entity*) const override;

private:
	int m_attackDamage;

protected:
	Tier& m_tier;
};
