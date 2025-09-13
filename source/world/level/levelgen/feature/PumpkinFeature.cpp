#include "Feature.hpp"
#include "world/level/Level.hpp"

bool PumpkinFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos actual;
	for(int var6 = 0; var6 < 64; ++var6) 
	{
		actual.x = pos.x + random->nextInt(8) - random->nextInt(8);
		actual.y = pos.y + random->nextInt(4) - random->nextInt(4);
		actual.z = pos.z + random->nextInt(8) - random->nextInt(8);
		if(level->isEmptyTile(actual) && level->getTile(actual.below()) == Tile::grass->m_ID && Tile::pumpkin->canSurvive(level, actual))
		{
			level->setTileAndDataNoUpdate(actual, Tile::pumpkin->m_ID, random->nextInt(4));
		}
	}

	return true;
}
