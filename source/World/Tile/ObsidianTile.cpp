#include "Tile.hpp"
#include "Level.hpp"

ObsidianTile::ObsidianTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int ObsidianTile::getResource(int data, Random* random)
{
	return Tile::obsidian->m_ID;
}

int ObsidianTile::getResourceCount(Random* random)
{
	return 1;
}
