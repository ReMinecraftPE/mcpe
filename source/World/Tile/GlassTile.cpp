#include "Tile.hpp"
#include "Level.hpp"

GlassTile::GlassTile(int a, int b, Material* c) : HalfTransparentTile(a, b, c)
{
}

int GlassTile::getResourceCount(Random* pRandom)
{
	return 0;
}