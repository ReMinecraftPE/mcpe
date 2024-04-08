/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RocketLauncherTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Rocket.hpp"

#define STATE_RECHARGING (0x1)
#define STATE_POWERED    (0x2)

RocketLauncherTile::RocketLauncherTile(int id) : Tile(id, 16*14+2, Material::wood)
{
}

int RocketLauncherTile::getTexture(int dir, int data)
{
	return (data & STATE_RECHARGING) ? 16*14+3 : 16*14+2;
}

AABB* RocketLauncherTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

int RocketLauncherTile::getRenderShape()
{
	return SHAPE_CROSS;
}

bool RocketLauncherTile::isCubeShaped()
{
	return false;
}

bool RocketLauncherTile::isSolidRender()
{
	return false;
}

bool RocketLauncherTile::isSignalSource()
{
	// Not really a signal _source_ per se, but it receives signals
	return true;
}

int RocketLauncherTile::use(Level* level, int x, int y, int z, Player* player)
{
	int data = level->getData(x, y, z);
	if (data & STATE_RECHARGING)
		return 1;

	level->setData(x, y, z, data | STATE_RECHARGING);

	// spawn a rocket
	level->addEntity(new Rocket(level, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f));

	// add a tick so that the rocket launcher will reset
	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());

	return 1;
}

void RocketLauncherTile::neighborChanged(Level* level, int x, int y, int z, int newTile)
{
	if (newTile <= 0 || !Tile::tiles[newTile]->isSignalSource())
		return;

	int data = level->getData(x, y, z);

	if (level->hasNeighborSignal(x, y, z))
	{
		if (data & (STATE_POWERED | STATE_RECHARGING))
			return;

		level->setDataNoUpdate(x, y, z, data | STATE_POWERED);
		use(level, x, y, z, nullptr);
	}
	else
	{
		if (data & STATE_POWERED)
			level->setDataNoUpdate(x, y, z, data & ~STATE_POWERED);
	}
}

void RocketLauncherTile::tick(Level* level, int x, int y, int z, Random* random)
{
	int data = level->getData(x, y, z);
	if (~data & STATE_RECHARGING)
		return;

	level->setData(x, y, z, data & ~STATE_RECHARGING);
}

int RocketLauncherTile::getTickDelay()
{
	return 10;
}
