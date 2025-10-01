/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SpongeTile.hpp"
#include "world/level/Level.hpp"

SpongeTile::SpongeTile(TileID id, int texture) : Tile(id, texture, Material::sponge)
{
}

void SpongeTile::onPlace(Level* level, const TilePos& pos)
{
	// get rid of all water in a 5x5 cube around this
	TilePos o;
	for (o.x = -2; o.x <= 2; o.x++)
	{
		for (o.y = -2; o.y <= 2; o.y++)
		{
			for (o.z = -2; o.z <= 2; o.z++)
			{
				if (level->getTile(pos + o) == Tile::water->m_ID ||
					level->getTile(pos + o) == Tile::calmWater->m_ID)
				{
					level->setTile(pos + o, TILE_AIR);
				}
			}
		}
	}
}

void SpongeTile::destroy(Level* level, const TilePos& pos, TileData data)
{
	// give an update to all water around us
	TilePos o;
	for (o.x = -3; o.x <= 3; o.x++)
	{
		for (o.y = -3; o.y <= 3; o.y++)
		{
			for (o.z = -3; o.z <= 3; o.z++)
			{
				if (level->getTile(pos + o) == Tile::water->m_ID ||
					level->getTile(pos + o) == Tile::calmWater->m_ID)
				{
					level->neighborChanged(pos + o, TILE_AIR);
				}
			}
		}
	}
}
