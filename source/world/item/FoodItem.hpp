#pragma once

#include "Item.hpp"

class FoodItem : public Item
{
public:
	FoodItem(int id, int nutrition);
	
	ItemStack* use(ItemStack*, Level*, Mob*) const override;

protected:
	int m_nutrition;
};