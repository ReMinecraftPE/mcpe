#pragma once

#include "Item.hpp"

class DyePowderItem : public Item
{
public:
    DyePowderItem(int);

	int getIcon(const ItemStack*) const override;
	std::string getDescriptionId() const override;
	std::string getDescriptionId(ItemStack*) const override;
	bool useOn(ItemStack*, Player*, Level*, const TilePos& pos, Facing::Name face) const override;
	void interactEnemy(ItemStack*, Mob*) const override;
};