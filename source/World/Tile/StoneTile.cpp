#include "Tile.hpp"
#include "Level.hpp"

StoneTile::StoneTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int StoneTile::getResource(int a, Random* b)
{
	return Tile::stoneBrick->m_ID;
}
