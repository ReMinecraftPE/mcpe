#include "Feature.hpp"
#include "world/level/Level.hpp"

bool CactusFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos tp;
	for(int var6 = 0; var6 < 10; ++var6) 
	{
		tp.x = pos.x + random->nextInt(8) - random->nextInt(8);
		tp.y = pos.y + random->nextInt(4) - random->nextInt(4);
		tp.z = pos.z + random->nextInt(8) - random->nextInt(8);
		if(level->isEmptyTile(tp)) {
			int var10 = 1 + random->nextInt(random->nextInt(3) + 1);
			for(int var11 = 0; var11 < var10; ++var11) {
				TilePos above = tp.above(var11);
				if(Tile::cactus->canSurvive(level, above)) {
					level->setTileNoUpdate(above, Tile::cactus->m_ID);
				}
			}
		}
	}

	return true;
}
