/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Item.hpp"
#include "Tile.hpp"
#include "Level.hpp"

TilePlanterItem::TilePlanterItem(int id, int place) : Item(id)
{
	m_tile = Tile::tiles[place]->m_ID;
}

bool TilePlanterItem::useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int dir)
{
	if (level->getTile(x, y, z) == Tile::topSnow->m_ID)
	{
		dir = DIR_YNEG;
	}
	else switch (dir)
	{
		case DIR_YNEG: y--; break;
		case DIR_YPOS: y++; break;
		case DIR_ZNEG: z--; break;
		case DIR_ZPOS: z++; break;
		case DIR_XNEG: x--; break;
		case DIR_XPOS: x++; break;
	}

	if (!instance->m_amount)
		return false;

	// why?
	if (!level->mayPlace(m_tile, x, y, z, false))
		return true;

	if (!level->setTile(x, y, z, m_tile))
		return true;

	Tile::tiles[m_tile]->setPlacedOnFace(level, x, y, z, dir);
	Tile::tiles[m_tile]->setPlacedBy(level, x, y, z, player);

	instance->m_amount--;
	return true;
}
