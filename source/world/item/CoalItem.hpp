#pragma once

#include "Item.hpp"

class CoalItem : public Item
{
public:
	CoalItem(int id);

public:
	std::string getDescriptionId(ItemStack* inst) const override;
};
