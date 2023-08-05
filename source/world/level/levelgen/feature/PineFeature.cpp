/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool PineFeature::place(Level* level, Random* random, int x, int y, int z)
{
	if (y <= C_MIN_Y)
		return false;

	int height = random->nextInt(5) + 7;
	if (height + y >= C_MAX_Y)
		return false;

	int x1 = height - random->nextInt(2) - 3;
	int p1 = height - x1;
	int x2 = 1 + random->nextInt(1 + x1);
	int upperY = y + 1 + height;

	bool bCanPlace = true;
	for (int i = 0, cy = y; cy <= upperY && bCanPlace; i++, cy++)
	{
		int range = 1;
		if (cy - y < x1)
			range = 0;
		else
			range = x2;

		for (int cx = x - range; cx <= x + range && bCanPlace; cx++)
		{
			for (int cz = z - range; cz <= z + range && bCanPlace; cz++)
			{
				if (cy >= C_MAX_Y || cy < C_MIN_Y)
				{
					bCanPlace = false;
					break;
				}

				TileID tile = level->getTile(cx, cy, cz);
				if (tile != TILE_AIR && tile != Tile::leaves->m_ID)
					bCanPlace = false;
			}
		}
	}

	if (!bCanPlace)
		return false;

	TileID tileBelow = level->getTile(x, y - 1, z);
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	if (y >= C_MAX_Y - 1 - height)
		return false;

	level->setTileNoUpdate(x, y - 1, z, Tile::dirt->m_ID);

	int range = 0;

	for (int cy = 0; cy <= height - x1; cy++)
	{
		int b1 = height + y - cy;
		for (int cx = x - range; cx <= x + range; cx++)
		{
			int dx = cx - x;
			for (int cz = z - range; cz <= z + range; cz++)
			{
				int dz = cz - z;
				if ((abs(dx) != range || abs(dz) != range || range <= 0) && !Tile::solid[level->getTile(cx, b1, cz)])
					level->setTileAndDataNoUpdate(cx, b1, cz, Tile::leaves->m_ID, 1);
			}
		}

		if (range >= 1 && cy == y + x1 + 1)
		{
			range--;
			continue;
		}
		if (range < x2)
		{
			range++;
			continue;
		}
	}

	int mheight = height - 1;
	for (int yd = 0; yd < mheight; yd++)
	{
		int cy = yd + y;

		TileID tile = level->getTile(x, cy, z);
		if (tile == TILE_AIR || tile == Tile::leaves->m_ID)
			level->setTileAndDataNoUpdate(x, cy, z, Tile::treeTrunk->m_ID, 1);
	}

	return true;
}