/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RocketItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/entity/Rocket.hpp"

RocketItem::RocketItem(int id) : Item(id)
{
}

bool RocketItem::useOn(ItemInstance* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
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

	level->addEntity(new Rocket(level, tp + 0.5f));

	inst->m_count--;
	return true;
}
