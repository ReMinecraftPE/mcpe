#pragma once

#include "../Item.hpp"
#include "../Inventory/CraftingContainer.hpp"

class Recipes
{
public:
	static int ANY_AUX_VALUE;
	static Recipes instance;

	//static Recipy[] recipes;


	Recipes();
	// Todo: Add CraftingContainer Class
	virtual ItemInstance getItemFor(CraftingContainer paramCraftingContainer);
	
private:
	virtual void addShapedRecipy(ItemInstance paramItemInstance, int paramVarArgs); // Todo: Add proper paramVarArg class
	virtual void addShapelessRecipy(ItemInstance paramItemInstace, int paramVarArgs);
};