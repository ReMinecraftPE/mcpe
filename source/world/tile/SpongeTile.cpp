/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

SpongeTile::SpongeTile(int id, int texture) : Tile(id, texture, Material::sponge)
{
}

void SpongeTile::onPlace(Level* level, int x, int y, int z)
{
	// get rid of all water in a 5x5 cube around this
	for (int ox = -2; ox <= 2; ox++)
	{
		for (int oy = -2; oy <= 2; oy++)
		{
			for (int oz = -2; oz <= 2; oz++)
			{
				if (level->getTile(x + ox, y + oy, z + oz) == Tile::water->m_ID ||
					level->getTile(x + ox, y + oy, z + oz) == Tile::calmWater->m_ID)
				{
					level->setTile(x + ox, y + oy, z + oz, TILE_AIR);
				}
			}
		}
	}
}

void SpongeTile::destroy(Level* level, int x, int y, int z, int dir)
{
	// give an update to all water around us
	for (int ox = -3; ox <= 3; ox++)
	{
		for (int oy = -3; oy <= 3; oy++)
		{
			for (int oz = -3; oz <= 3; oz++)
			{
				if (level->getTile(x + ox, y + oy, z + oz) == Tile::water->m_ID ||
					level->getTile(x + ox, y + oy, z + oz) == Tile::calmWater->m_ID)
				{
					level->neighborChanged(x + ox, y + oy, z + oz, TILE_AIR);
				}
			}
		}
	}
}
