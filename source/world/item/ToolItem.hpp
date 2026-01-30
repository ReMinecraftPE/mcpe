#pragma once

#include <vector>
#include "Item.hpp"

class ToolItem : public Item
{
public:
	ToolItem(int id, int attackDamageBase, Tier& tier);

	float getDestroySpeed(ItemStack* instance, const Tile* tile) const;
	void hurtEnemy(ItemStack* instance, Mob* mob) const override;
	void mineBlock(ItemStack*, const TilePos& pos, Facing::Name face, Player* player) const override;
	int getAttackDamage(Entity* entity) const override;
	bool isHandEquipped() const override;

protected:
	void initializeTiles(const Tile** tiles, int tileCount);

private:
	float m_speed;
	int m_attackDamage;

protected:
	Tier& m_tier;

	std::vector<const Tile*> m_tiles;
};
