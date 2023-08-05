/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool BirchFeature::place(Level* level, Random* random, int x, int y, int z)
{
	if (y <= C_MIN_Y)
		return false;

	int treeHeight = int(random->nextInt(3) + 5); // Between 5 and 7 blocks tall.

	if (y + treeHeight >= C_MAX_Y)
		return false;

	// Ensure that we can place this tree
	bool bCanPlace = true;
	for (int ay = y; ay <= y + treeHeight; ay++)
	{
		int x1 = 1;
		if (y + treeHeight - 1 <= ay)
			x1 = 2;
		else if (ay == y)
			x1 = 0;

		for (int ax = x - x1; ax <= x + x1 && bCanPlace; ax++)
		{
			for (int az = z - x1; az <= z + x1 && bCanPlace; az++)
			{
				if (ay < C_MIN_Y || ay >= C_MAX_Y)
				{
					bCanPlace = false;
					break;
				}

				TileID tile = level->getTile(ax, ay, az);

				// other trees can overlap with this one, apparently
				if (tile != TILE_AIR && tile != Tile::leaves->m_ID)
				{
					bCanPlace = false;
					break;
				}
			}
		}
	}

	// If not, let our caller know.
	if (!bCanPlace)
		return false;

	TileID tileBelow = level->getTile(x, y - 1, z);

	// If grass or dirt aren't below us, we can't possibly grow!
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	// @NOTE: Redundant check
	if (y >= C_MAX_Y - treeHeight)
		return false;

	level->setTileNoUpdate(x, y - 1, z, Tile::dirt->m_ID);

	int upperY = y + treeHeight;
	int lowerY = y + treeHeight - 3;
	int diff = lowerY - upperY;

	for (int i = lowerY; i <= upperY; i++, diff = i - upperY)
	{
		int c1 = 1 - diff / 2;
		int c2 = diff / 2 - 1;
		for (int ax = x - c1; ax <= x + c1; ax++)
		{
			int c3 = c2;
			int c4 = diff / 2 - 1;
			if (c2 < 0)
				c3 = -c2;

			int c5 = c3;

			for (int az = z - c1; az <= z + c1; az++, c4++)
			{
				if ((abs(ax - x) != c1 || abs(az - z) != c1 || random->nextInt(2) != 0 && diff != 0) && !Tile::solid[level->getTile(ax, i, az)])
				{
					level->setTileAndDataNoUpdate(ax, i, az, Tile::leaves->m_ID, 2);
				}
			}
		}
	}

	for (int i = 0; i < treeHeight; i++)
	{
		int y1 = i + y;
		TileID tile = level->getTile(x, y + i, z);
		if (tile && tile != Tile::leaves->m_ID)
			continue;

		level->setTileAndDataNoUpdate(x, y + i, z, Tile::treeTrunk->m_ID, 2);
	}

	return true;
}
