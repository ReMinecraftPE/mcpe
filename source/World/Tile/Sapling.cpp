#include "Tile.hpp"
#include "Level.hpp"

Sapling::Sapling(int id, int texture) : Bush(id, texture)
{
}

int Sapling::getTexture(int dir, int data)
{
	return TEXTURE_SAPLING; // we don't have the other saplings' textures...
}

void Sapling::tick(Level* level, int x, int y, int z, Random* random)
{
	Bush::tick(level, x, y, z, random);

	if (level->getRawBrightness(x, y, z) > 8 && random->nextInt(7) == 0)
	{
		int data = level->getData(x, y, z);

		if (data & 8)
			growTree(level, x, y, z, random);
		else
			level->setDataNoUpdate(x, y, z, data | 8);
	}
}

bool Sapling::maybeGrowTree(Level* level, int x, int y, int z, Random* random)
{
	// this is fine... these are not heavy at all
	TreeFeature treeFeature;
	BirchFeature birchFeature;
	SpruceFeature spruceFeature;

	Feature* pFeature = &treeFeature;

	int data = level->getData(x, y, z);
	switch (data)
	{
		case 1:
			pFeature = &birchFeature;
			break;
		case 2:
			pFeature = &spruceFeature;
			break;
	}

	return treeFeature.place(level, random, x, y, z);
}

void Sapling::growTree(Level* level, int x, int y, int z, Random* random)
{
	level->setTileNoUpdate(x, y, z, TILE_AIR);

	if (!maybeGrowTree(level, x, y, z, random))
		level->setTileNoUpdate(x, y, z, m_ID);
}
