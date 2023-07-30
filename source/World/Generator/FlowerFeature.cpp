#include "Feature.hpp"
#include "Level.hpp"

FlowerFeature::FlowerFeature(int id)
{
	m_ID = id;
}

bool FlowerFeature::place(Level* level, Random* random, int x, int y, int z)
{
	for (int i = 0; i < 64; i++)
	{
		int nX = x + random->nextInt(8) - random->nextInt(8);
		int nY = y + random->nextInt(4) - random->nextInt(4);
		int nZ = z + random->nextInt(8) - random->nextInt(8);

		if (level->isEmptyTile(nX, nY, nZ) && Tile::tiles[m_ID]->canSurvive(level, nX, nY, nZ))
			level->setTileNoUpdate(nX, nY, nZ, m_ID);
	}

	return true;
}
