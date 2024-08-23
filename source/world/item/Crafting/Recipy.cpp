#pragma once
#include "Recipy.hpp"
#include "../ItemInstance.hpp"
int size()
{
	return 0;
};

ItemInstance assemble(CraftingContainer paramCraftingContainer)
{
	return *(new ItemInstance(0,0,0));
};


bool matches(CraftingContainer paramCraftingContainer)
{
	return false;
};