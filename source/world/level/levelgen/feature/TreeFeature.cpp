/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool TreeFeature::place(Level* level, Random* random, const TilePos& pos)
{
	if (pos.y <= C_MIN_Y)
		return false;

	int treeHeight = int(random->nextInt(3) + 4); // Between 4 and 6 blocks tall.

	if (pos.y + treeHeight >= C_MAX_Y)
		return false;

	// Ensure that we can place this tree
	bool bCanPlace = true;
	TilePos tp;
	for (tp.y = pos.y; tp.y <= pos.y + treeHeight; tp.y++)
	{
		int x1 = 1;
		if (pos.y + treeHeight - 1 <= tp.y)
			x1 = 2;
		else if (tp.y == pos.y)
			x1 = 0;

		for (tp.x = pos.x - x1; tp.x <= pos.x + x1 && bCanPlace; tp.x++)
		{
			for (tp.z = pos.z - x1; tp.z <= pos.z + x1 && bCanPlace; tp.z++)
			{
				if (tp.y < C_MIN_Y || tp.y >= C_MAX_Y)
				{
					bCanPlace = false;
					break;
				}

				TileID tile = level->getTile(tp);

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

	TileID tileBelow = level->getTile(pos.below());

	// If grass or dirt aren't below us, we can't possibly grow!
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	// @NOTE: Redundant check
	if (pos.y >= C_MAX_Y - treeHeight)
		return false;

	level->setTileNoUpdate(pos.below(), Tile::dirt->m_ID);

	int upperY = pos.y + treeHeight;
	int lowerY = pos.y + treeHeight - 3;
	int diff = lowerY - upperY;

	for (tp.y = lowerY; tp.y <= upperY; tp.y++, diff = tp.y - upperY)
	{
		int c1 = 1 - diff / 2;
		int c2 = diff / 2 - 1;
		for (tp.x = pos.x - c1; tp.x <= pos.x + c1; tp.x++)
		{
			int c3 = c2;
			int c4 = diff / 2 - 1;
			if (c2 < 0)
				c3 = -c2;

			//int c5 = c3;

			for (tp.z = pos.z - c1; tp.z <= pos.z + c1; tp.z++, c4++)
			{
                if ((abs(tp.x - pos.x) != c1 || abs(tp.z - pos.z) != c1 || (random->nextInt(2) != 0 && diff != 0)) && !Tile::solid[level->getTile(tp)])
				{
					level->setTileNoUpdate(tp, Tile::leaves->m_ID);
				}
			}
		}
	}

	for (int y = 0; y < treeHeight; y++)
	{
		TilePos t(pos);
		t.y += y;
		TileID tile = level->getTile(t);
		if (tile && tile != Tile::leaves->m_ID)
			continue;

		level->setTileNoUpdate(t, Tile::treeTrunk->m_ID);
	}

	return true;
}
