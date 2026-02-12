#pragma once

#include "Item.hpp"

class SeedItem : public Item
{
public:
	SeedItem(int id, int place);

	virtual bool useOn(ItemStack*, Player*, Level*, const TilePos& pos, Facing::Name face) const;
public:
	int m_tile;
};
