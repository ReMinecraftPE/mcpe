/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Bush.hpp"
#include "world/level/Level.hpp"

Bush::Bush(TileID id, int texture) : Tile(id, Material::plant)
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

bool Bush::mayPlace(const Level* level, const TilePos& pos) const
{
	TileID tile = level->getTile(pos.below());

	return tile == Tile::grass->m_ID || tile == Tile::dirt->m_ID || tile == Tile::farmland->m_ID;
}

bool Bush::canSurvive(const Level* level, const TilePos& pos) const
{
	if (level->getRawBrightness(pos) <= 7 && !level->canSeeSky(pos))
		return false;

	return mayPlace(level, pos);
}

void Bush::checkAlive(Level* level, const TilePos& pos)
{
	if (!canSurvive(level, pos))
		level->setTile(pos, TILE_AIR);
}

void Bush::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	return checkAlive(level, pos);
}

void Bush::tick(Level* level, const TilePos& pos, Random* random)
{
	checkAlive(level, pos);
}

AABB* Bush::getAABB(const Level* level, const TilePos& pos)
{
	return nullptr;
}
