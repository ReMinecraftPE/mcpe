#pragma once

#include "ToolItem.hpp"

class HoeItem : public Item
{
public:
	HoeItem(int id, ToolItem::Tier& tier);

	bool useOn(ItemStack*, Player*, Level*, const TilePos& pos, Facing::Name face) const override;
	bool isHandEquipped() const override;
};
