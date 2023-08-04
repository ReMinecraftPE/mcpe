/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "Level.hpp"

bool ReedsFeature::place(Level* level, Random* random, int x, int y, int z)
{
	for (int i = 0; i < 20; i++)
	{
		int newX = (x + random->nextInt(4)) - random->nextInt(4);
		int newZ = (z + random->nextInt(4)) - random->nextInt(4);

		if (!level->isEmptyTile(newX, y, newZ))
			continue;

		if (level->getMaterial(newX - 1, y - 1, newZ) == Material::water ||
			level->getMaterial(newX + 1, y - 1, newZ) == Material::water ||
			level->getMaterial(newX, y - 1, newZ - 1) == Material::water ||
			level->getMaterial(newX, y - 1, newZ + 1) == Material::water)
		{
			int height = random->nextInt(random->nextInt(3) + 1) + 2;
			for (int y1 = 0; y1 < height; y1++)
			{
				if (Tile::reeds->canSurvive(level, newX, y + y1, newZ))
				{
					level->setTileNoUpdate(newX, y + y1, newZ, Tile::reeds->m_ID);
				}
			}
		}
	}

	return true;
}
