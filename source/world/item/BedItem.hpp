#pragma once

#include "Item.hpp"

class BedItem : public Item
{
public:
	BedItem(int id);

	bool useOn(ItemStack*, Player*, Level*, const TilePos& pos, Facing::Name face) const override;
};
