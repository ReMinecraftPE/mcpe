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

bool RocketItem::useOn(ItemInstance* inst, Player* player, Level* level, int x, int y, int z, int dir)
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

	level->addEntity(new Rocket(level, float(x), float(y), float(z)));

	inst->m_amount--;
	return true;
}
