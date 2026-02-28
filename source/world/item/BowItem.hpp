#pragma once

#include "Item.hpp"

class BowItem : public Item
{
public:
	BowItem(int id);

public:
	ItemStack* use(ItemStack* inst, Level* level, Mob* user) const override;
};
