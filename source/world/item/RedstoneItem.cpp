/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RedstoneItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"

RedstoneItem::RedstoneItem(int id) : Item(id)
{
}

bool RedstoneItem::useOn(ItemInstance* inst, Player* player, Level* level, int x, int y, int z, int dir)
{
	if (level->getTile(x, y, z) != Tile::topSnow->m_ID)
	{
		switch (dir)
		{
			case DIR_YNEG: y--; break;
			case DIR_YPOS: y++; break;
			case DIR_ZNEG: z--; break;
			case DIR_ZPOS: z++; break;
			case DIR_XNEG: x--; break;
			case DIR_XPOS: x++; break;
		}
	}

	if (level->getTile(x, y, z) != TILE_AIR)
		return false;

	if (Tile::wire->mayPlace(level, x, y, z))
	{
		level->setTile(x, y, z, Tile::wire->m_ID);
		inst->m_amount--;
	}

	return true;
}
