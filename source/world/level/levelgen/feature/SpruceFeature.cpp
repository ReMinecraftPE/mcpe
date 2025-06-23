/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool SpruceFeature::place(Level* level, Random* random, const TilePos& pos)
{
	if (pos.y <= C_MIN_Y)
		return false;

	int height = random->nextInt(4) + 6;
	if (height + pos.y >= C_MAX_Y)
		return false;

	int x1 = random->nextInt(2) + 1;
	int x2 = random->nextInt(2) + 2;
	int upperY = pos.y + 1 + height;
	
	bool bCanPlace = true;
	for (int i = 0, cy = pos.y; cy <= upperY && bCanPlace; i++, cy++)
	{
		int range = x2;
		if (x1 <= i)
			range = 0;

		for (int cx = pos.x - range; cx <= pos.x + range && bCanPlace; cx++)
		{
			for (int cz = pos.z - range; cz <= pos.z + range && bCanPlace; cz++)
			{
				if (cy >= C_MAX_Y || cy < C_MIN_Y)
				{
					bCanPlace = false;
					break;
				}

				TileID tile = level->getTile(TilePos(cx, cy, cz));
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

	int range = random->nextInt(2);
	int b2 = 1, b3 = 0;

	TilePos tp(pos.x - range, 0, pos.z - range);
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

		if (range >= b2)
		{
			b2++;
			range = b3;
			if (b2 > x2)
				b2 = x2;
			b3 = 1;
		}
		else
		{
			range++;
		}
	}

	int mheight = height - random->nextInt(3);
	for (int yd = 0; yd < mheight; yd++)
	{
		int cy = yd + pos.y;
		
		TileID tile = level->getTile(TilePos(pos.x, cy, pos.z));
		if (tile == TILE_AIR || tile == Tile::leaves->m_ID)
			level->setTileAndDataNoUpdate(TilePos(pos.x, cy, pos.z), Tile::treeTrunk->m_ID, 1);
	}

	return true;
}