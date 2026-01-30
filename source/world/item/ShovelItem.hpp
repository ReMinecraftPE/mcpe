#pragma once

#include "ToolItem.hpp"

class ShovelItem : public ToolItem
{
public:
	ShovelItem(int id, Tier& tier);

	bool canDestroySpecial(const Tile* tile) const override;
};
