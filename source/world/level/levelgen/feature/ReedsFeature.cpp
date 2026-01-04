/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool ReedsFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos tp(pos);

	for (int i = 0; i < 20; i++)
	{
		tp.x = pos.x + random->nextInt(4) - random->nextInt(4);
		tp.z = pos.z + random->nextInt(4) - random->nextInt(4);

		if (!level->isEmptyTile(tp))
			continue;

		if (level->getMaterial(TilePos(tp.x - 1, tp.y - 1, tp.z)) == Material::water ||
			level->getMaterial(TilePos(tp.x + 1, tp.y - 1, tp.z)) == Material::water ||
			level->getMaterial(TilePos(tp.x, tp.y - 1, tp.z - 1)) == Material::water ||
			level->getMaterial(TilePos(tp.x, tp.y - 1, tp.z + 1)) == Material::water)
		{
			int height = random->nextInt(random->nextInt(3) + 1) + 2;
			for (int y1 = 0; y1 < height; y1++)
			{
				if (Tile::reeds->canSurvive(level, TilePos(tp.x, tp.y + y1, tp.z)))
				{
					level->setTileNoUpdate(TilePos(tp.x, tp.y + y1, tp.z), Tile::reeds->m_ID);
				}
			}
		}
	}

	return true;
}
