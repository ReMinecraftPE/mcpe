/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Bush.hpp"
#include "world/level/Level.hpp"

Bush::Bush(int id, int texture) : Tile(id, Material::plant)
{
	m_TextureFrame = texture;
	setTicking(true);
	setShape(0.3f, 0.0f, 0.3f, 0.7f, 0.6f, 0.7f);
}

int Bush::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool Bush::isCubeShaped() const
{
	return false;
}

bool Bush::isSolidRender() const
{
	return false;
}

bool Bush::mayPlace(const Level* level, int x, int y, int z) const
{
	TileID tile = level->getTile(x, y - 1, z);

	return tile == Tile::grass->m_ID || tile == Tile::dirt->m_ID || tile == Tile::farmland->m_ID;
}

bool Bush::canSurvive(const Level* level, int x, int y, int z) const
{
	if (level->getRawBrightness(x, y, z) <= 7 && !level->canSeeSky(x, y, z))
		return false;

	return mayPlace(level, x, y, z);
}

void Bush::checkAlive(Level* level, int x, int y, int z)
{
	if (!canSurvive(level, x, y, z))
		level->setTile(x, y, z, TILE_AIR);
}

void Bush::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	return checkAlive(level, x, y, z);
}

void Bush::tick(Level* level, int x, int y, int z, Random* random)
{
	checkAlive(level, x, y, z);
}

AABB* Bush::getAABB(const Level* level, int x, int y, int z)
{
	return nullptr;
}
