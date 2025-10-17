/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RocketLauncherTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Rocket.hpp"

RocketLauncherTile::RocketLauncherTile(TileID id) : Tile(id, 16*14+2, Material::wood)
{
	setTicking(true);
}

int RocketLauncherTile::getTexture(Facing::Name face, TileData data) const
{
	return data == 1 ? 16*14+3 : 16*14+2;
}

AABB* RocketLauncherTile::getAABB(const Level*, const TilePos& pos)
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

bool RocketLauncherTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (level->getData(pos) == 1)
		return true;

	level->setData(pos, 1);

	// spawn a rocket
	level->addEntity(new Rocket(level, Vec3(pos) + 0.5f));

	// add a tick so that the rocket launcher will reset
	level->addToTickNextTick(pos, m_ID, 10);

	return true;
}

void RocketLauncherTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->getData(pos) != 1)
		return;

	level->setData(pos, 0);
}
