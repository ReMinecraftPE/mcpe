#include "Tile.hpp"
#include "Level.hpp"

GravelTile::GravelTile(int a, int b, Material* c) : SandTile(a, b, c)
{
}

int GravelTile::getResource(int a, Random* b)
{
	return m_ID;
}
