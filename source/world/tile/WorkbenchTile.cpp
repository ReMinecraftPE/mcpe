#include "WorkbenchTile.hpp"
#include "Tile.hpp"
#include "../level/Level.hpp"
WorkBenchTile::WorkBenchTile(int id, Material *) : Tile(id, TEXTURE_PLANKS, Material::wood)
{

} 

int WorkBenchTile::getTexture(int paramDir)
{
	if (paramDir == DIR_YNEG)
		return TEXTURE_PLANKS;
	if (paramDir == DIR_YPOS)
	{
		return TEXTURE_WORKBENCH_TOP;
	}
	if (paramDir == DIR_ZNEG || DIR_XNEG)
	{
		return TEXTURE_WORKBENCH_SIDE_2;
	}
	return TEXTURE_WORKBENCH_SIDE_1;
}

int WorkBenchTile::use(Level* pLevel, int x, int y, int z, Player* player)
{
	player->startCrafting(x, y, z);
	return 1;
}