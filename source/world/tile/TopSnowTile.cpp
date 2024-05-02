/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TopSnowTile.hpp"
#include "world/level/Level.hpp"

TopSnowTile::TopSnowTile(int a, int b, Material* c) : Tile(a, b, c)
{
	setShape(0, 0, 0, 1, 0.125f, 1);
	setTicking(true);
}

AABB* TopSnowTile::getAABB(const Level*, int x, int y, int z)
{
	return nullptr;
}

bool TopSnowTile::isCubeShaped() const
{
	return false;
}

bool TopSnowTile::isSolidRender() const
{
	return false;
}

int TopSnowTile::getResource(int x, Random* random) const
{
	return 0;
}

int TopSnowTile::getResourceCount(Random* random) const
{
	return 0;
}

bool TopSnowTile::mayPlace(const Level* level, int x, int y, int z) const
{
	TileID tile = level->getTile(x, y - 1, z);

	if (!tile || !Tile::tiles[tile]->isSolidRender())
		return false;

	return level->getMaterial(x, y - 1, z)->blocksMotion();
}

bool TopSnowTile::checkCanSurvive(Level* level, int x, int y, int z)
{
	if (mayPlace(level, x, y, z))
		return true;

	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
	return false;
}

void TopSnowTile::neighborChanged(Level* level, int x, int y, int z, int d)
{
	checkCanSurvive(level, x, y, z);
}

bool TopSnowTile::shouldRenderFace(const LevelSource* level, int x, int y, int z, int dir) const
{
	if (dir == DIR_YPOS)
		return true;

	if (level->getMaterial(x,y,z) == m_pMaterial)
		return false;

	return Tile::shouldRenderFace(level, x, y, z, dir);
}

void TopSnowTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (level->getBrightness(LightLayer::Block, x, y, z) > 11)
	{
		spawnResources(level, x, y, z, level->getData(x, y, z));
		level->setTile(x, y, z, TILE_AIR);
	}
}
