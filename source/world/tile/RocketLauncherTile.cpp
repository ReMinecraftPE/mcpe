/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RocketLauncherTile.hpp"
#include "world/item/RocketItem.hpp"

RocketLauncherTile::RocketLauncherTile(int id, int texture) : Tile(id, texture, Material::wood)
{
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

int RocketLauncherTile::use(Level* pLevel, int x, int y, int z, Player* player)
{
	// spawn a rocket


	return 1;
}
