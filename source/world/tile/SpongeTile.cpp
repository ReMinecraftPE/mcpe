/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SpongeTile.hpp"
#include "world/level/TileSource.hpp"

SpongeTile::SpongeTile(TileID id, int texture) : Tile(id, texture, Material::sponge)
{
}

void SpongeTile::onPlace(TileSource* source, const TilePos& pos)
{
	// get rid of all water in a 5x5 cube around this
	TilePos o;
	for (o.x = -2; o.x <= 2; o.x++)
	{
		for (o.y = -2; o.y <= 2; o.y++)
		{
			for (o.z = -2; o.z <= 2; o.z++)
			{
				if (source->getTile(pos + o) == Tile::water->m_ID ||
					source->getTile(pos + o) == Tile::calmWater->m_ID)
				{
					source->setTile(pos + o, TILE_AIR);
				}
			}
		}
	}
}

void SpongeTile::destroy(TileSource* source, const TilePos& pos, TileData data)
{
	// give an update to all water around us
	TilePos o;
	for (o.x = -3; o.x <= 3; o.x++)
	{
		for (o.y = -3; o.y <= 3; o.y++)
		{
			for (o.z = -3; o.z <= 3; o.z++)
			{
				if (source->getTile(pos + o) == Tile::water->m_ID ||
					source->getTile(pos + o) == Tile::calmWater->m_ID)
				{
					// TODO
					//level->neighborChanged(pos + o, TILE_AIR);
				}
			}
		}
	}
}
