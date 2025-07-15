/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool PineFeature::place(Level* level, Random* random, const TilePos& pos)
{
	if (pos.y <= C_MIN_Y)
		return false;

	int height = random->nextInt(5) + 7;
	if (height + pos.y >= C_MAX_Y)
		return false;

	int x1 = height - random->nextInt(2) - 3;
	//int p1 = height - x1;
	int x2 = 1 + random->nextInt(1 + x1);
	int upperY = pos.y + 1 + height;

	bool bCanPlace = true;
	TilePos tp(pos);
	for (tp.y = pos.y; tp.y <= upperY && bCanPlace; tp.y++)
	{
		int range = 1;
		if (tp.y - pos.y < x1)
			range = 0;
		else
			range = x2;

		for (tp.x = pos.x - range; tp.x <= pos.x + range && bCanPlace; tp.x++)
		{
			for (tp.z = pos.z - range; tp.z <= pos.z + range && bCanPlace; tp.z++)
			{
				if (tp.y >= C_MAX_Y || tp.y < C_MIN_Y)
				{
					bCanPlace = false;
					break;
				}

				TileID tile = level->getTile(tp);
				if (tile != TILE_AIR && tile != Tile::leaves->m_ID)
					bCanPlace = false;
			}
		}
	}

	if (!bCanPlace)
		return false;

	TileID tileBelow = level->getTile(pos.below());
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	if (pos.y >= C_MAX_Y - 1 - height)
		return false;

	level->setTileNoUpdate(pos.below(), Tile::dirt->m_ID);

	int range = 0;

	for (tp.y = 0; tp.y <= height - x1; tp.y++)
	{
		int b1 = height + pos.y - tp.y;
		for (tp.x = pos.x - range; tp.x <= pos.x + range; tp.x++)
		{
			int dx = tp.x - pos.x;
			for (tp.z = pos.z - range; tp.z <= pos.z + range; tp.z++)
			{
				int dz = tp.z - pos.z;
				if ((abs(dx) != range || abs(dz) != range || range <= 0) && !Tile::solid[level->getTile(TilePos(tp.x, b1, tp.z))])
					level->setTileAndDataNoUpdate(TilePos(tp.x, b1, tp.z), Tile::leaves->m_ID, 1);
			}
		}

		if (range >= 1 && tp.y == pos.y + x1 + 1)
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
		int cy = yd + pos.y;

		TileID tile = level->getTile(TilePos(pos.x, cy, pos.z));
		if (tile == TILE_AIR || tile == Tile::leaves->m_ID)
			level->setTileAndDataNoUpdate(TilePos(pos.x, cy, pos.z), Tile::treeTrunk->m_ID, 1);
	}

	return true;
}
