#pragma once

#include "Item.hpp"

class BowItem : public Item
{
public:
	BowItem(int id);

public:
	ItemStack* use(ItemStack* inst, Mob* user) const override;
};
