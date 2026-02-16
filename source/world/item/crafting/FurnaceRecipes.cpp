#include "FurnaceRecipes.hpp"
#include "common/Logger.hpp"

FurnaceRecipes* FurnaceRecipes::instance;

FurnaceRecipes::FurnaceRecipes()
{
	addFurnaceRecipe(Tile::ironOre,		 ItemStack(Item::ironIngot));
	addFurnaceRecipe(Tile::goldOre,		 ItemStack(Item::goldIngot));
	addFurnaceRecipe(Tile::emeraldOre,   ItemStack(Item::emerald));
	addFurnaceRecipe(Tile::coalOre,		 ItemStack(Item::coal));
	addFurnaceRecipe(Tile::redStoneOre,  ItemStack(Item::redStone, 4));
	addFurnaceRecipe(Tile::lapisOre,	 ItemStack(Item::dye_powder, 4, 4));
	addFurnaceRecipe(Tile::sand,		 ItemStack(Tile::glass));
	addFurnaceRecipe(Item::porkChop_raw, ItemStack(Item::porkChop_cooked));
	addFurnaceRecipe(Item::fish_raw,	 ItemStack(Item::fish_cooked));
	addFurnaceRecipe(Tile::stoneBrick,   ItemStack(Tile::rock));
	addFurnaceRecipe(Item::clay,		 ItemStack(Item::brick));
	addFurnaceRecipe(Tile::cactus,		 ItemStack(Item::dye_powder, 1, 2));
	addFurnaceRecipe(Tile::treeTrunk,    ItemStack(Item::coal, 1, 1));

    LOG_I("%d furnace recipes loaded", m_furnaceRecipes.size());

    addFurnaceFuel(Material::wood,	  300);
    addFurnaceFuel(Item::stick,		  100);
    addFurnaceFuel(Item::coal,	      1600);
    addFurnaceFuel(Item::bucket_lava, 20000);
}

FurnaceRecipes::~FurnaceRecipes()
{
	for (FurnaceRecipesMap::iterator it = m_furnaceRecipes.begin(); it != m_furnaceRecipes.end(); ++it)
	{
		delete it->second;
	}
}
