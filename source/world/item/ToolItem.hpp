#pragma once

#include <vector>
#include "Item.hpp"
#include "Tool.hpp"

class ToolItem : public Item
{
public:
	ToolItem(int id, Tool::Type toolType, Item::Tier& tier);

	float getDestroySpeed(ItemStack* instance, const Tile* tile) const;
	void hurtEnemy(ItemStack* instance, Mob* mob) const override;
	void mineBlock(ItemStack*, const TilePos& pos, Facing::Name face, Mob* mob) const override;
	int getAttackDamage(Entity* entity) const override;
	bool isHandEquipped() const override;
	bool canDestroySpecial(const Tile* tile) const override;

private:
	float m_speed;
	Tool::Type m_toolType;
	Tier& m_tier;
};