#include "Recipes.hpp"
#include "../Inventory/CraftingContainer.hpp";
Recipes::Recipes()
{

}

ItemInstance Recipes::getItemFor(CraftingContainer paramCraftingContainer)
{
	return *(new ItemInstance(0, 0, 0));
}

void Recipes::addShapedRecipy(ItemInstance paramItemInstance, int paramVarArgs)
{

}
void Recipes::addShapelessRecipy(ItemInstance paramItemInstance, int paramVarArgs)
{

}
