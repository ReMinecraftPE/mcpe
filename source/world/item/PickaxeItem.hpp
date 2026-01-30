#pragma once

#include "ToolItem.hpp"

class PickaxeItem : public ToolItem
{
public:
	PickaxeItem(int id, Tier& tier);
	bool canDestroySpecial(const Tile* tile) const override;
};