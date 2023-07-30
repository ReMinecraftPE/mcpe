#include "Tile.hpp"
#include "Level.hpp"

OreTile::OreTile(int id, int texture) : Tile(id, texture, Material::stone)
{

}

int OreTile::getResource(int x, Random* random)
{
	return m_ID;
}

int OreTile::getResourceCount(Random* random)
{
	if (m_ID == Tile::lapisOre->m_ID)
		return random->genrand_int32() % 5 + 4;

	return 1;
}

int OreTile::getSpawnResourcesAuxValue(int x)
{
	return 0;
}
