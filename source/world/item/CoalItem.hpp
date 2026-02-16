#pragma once

#include "Item.hpp"

class CoalItem : public Item
{
public:
	CoalItem(int id);

	std::string getDescriptionId(ItemStack* inst) const override;

};