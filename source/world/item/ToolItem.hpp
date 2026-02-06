#pragma once

#include <vector>
#include "Item.hpp"
#include "Tool.hpp"

class ToolItem : public Item
{
public:
	ToolItem(int id, Tool::Type toolType, Tier& tier);

protected:
	bool _canDestroyTile(const Tile*) const;
	bool _canDestroyMaterial(const Material*) const;

public:
	float getDestroySpeed(ItemStack* instance, const Tile* tile) const override;
	void hurtEnemy(ItemStack* instance, Mob* mob) const override;
	void mineBlock(ItemStack*, const TilePos& pos, Facing::Name face, Mob* mob) const override;
	int getAttackDamage(Entity* entity) const override;
	bool isHandEquipped() const override;
	bool canDestroySpecial(const Tile* tile) const override;

private:
	float m_speed;
	Tier& m_tier;

protected:
	Tool::Type m_toolType;
};