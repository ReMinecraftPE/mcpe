#pragma once

#include "Item.hpp"

class SeedItem : public Item
{
public:
	SeedItem(int id, int place);

	bool useOn(ItemStack*, Player*, const TilePos& pos, Facing::Name face) const override;
public:
	int m_tile;
};
