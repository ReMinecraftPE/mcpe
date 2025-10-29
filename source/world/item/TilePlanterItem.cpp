/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TilePlanterItem.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

TilePlanterItem::TilePlanterItem(int id, int place) : Item(id)
{
	m_tile = Tile::tiles[place]->m_ID;
}

bool TilePlanterItem::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
	TilePos tp(pos);

	if (level->getTile(pos) == Tile::topSnow->m_ID)
	{
		face = Facing::DOWN;
	}
	else switch (face)
	{
		case Facing::DOWN: tp.y--; break;
		case Facing::UP: tp.y++; break;
		case Facing::NORTH: tp.z--; break;
		case Facing::SOUTH: tp.z++; break;
		case Facing::WEST: tp.x--; break;
		case Facing::EAST: tp.x++; break;
	}

	if (!instance->m_count)
		return false;

	// why?
	if (!level->mayPlace(m_tile, tp, false))
		return true;

	if (!level->setTile(tp, m_tile))
		return true;

	Tile::tiles[m_tile]->setPlacedOnFace(level, tp, face);
	Tile::tiles[m_tile]->setPlacedBy(level, tp, player);

	instance->m_count--;
	return true;
}
