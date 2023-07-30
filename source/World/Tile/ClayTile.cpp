#include "Tile.hpp"
#include "Level.hpp"

ClayTile::ClayTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int ClayTile::getResource(int, Random* random)
{
	return 0; //@NOTE: Would be clay's item ID
}

int ClayTile::getResourceCount(Random* random)
{
	return 4;
}
