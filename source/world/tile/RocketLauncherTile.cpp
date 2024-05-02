/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RocketLauncherTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Rocket.hpp"

RocketLauncherTile::RocketLauncherTile(int id) : Tile(id, 16*14+2, Material::wood)
{
	setTicking(true);
}

int RocketLauncherTile::getTexture(int dir, int data) const
{
	return data == 1 ? 16*14+3 : 16*14+2;
}

AABB* RocketLauncherTile::getAABB(const Level*, int x, int y, int z)
{
	return nullptr;
}

int RocketLauncherTile::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool RocketLauncherTile::isCubeShaped() const
{
	return false;
}

bool RocketLauncherTile::isSolidRender() const
{
	return false;
}

int RocketLauncherTile::use(Level* level, int x, int y, int z, Player* player)
{
	if (level->getData(x, y, z) == 1)
		return 1;

	level->setData(x, y, z, 1);

	// spawn a rocket
	level->addEntity(new Rocket(level, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f));

	// add a tick so that the rocket launcher will reset
	level->addToTickNextTick(x, y, z, m_ID, 10);

	return 1;
}

void RocketLauncherTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (level->getData(x, y, z) != 1)
		return;

	level->setData(x, y, z, 0);
}
