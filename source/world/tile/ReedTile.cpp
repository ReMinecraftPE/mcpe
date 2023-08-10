/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ReedTile.hpp"
#include "world/level/Level.hpp"

ReedTile::ReedTile(int id) : Tile(id, Material::plant)
{
	m_TextureFrame = TEXTURE_REEDS;
	setShape(0.125f, 0.0f, 0.125f, 0.875f, 1.0f, 0.875f);
	setTicking(true);
}

int ReedTile::getRenderShape()
{
	return 1;
}

bool ReedTile::isCubeShaped()
{
	return false;
}

bool ReedTile::isSolidRender()
{
	return false;
}

bool ReedTile::mayPlace(Level* level, int x, int y, int z)
{
	TileID tileBelow = level->getTile(x, y - 1, z);

	if (tileBelow == m_ID)
		return true;

	//@NOTE: No sand
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	return
		level->getMaterial(x - 1, y - 1, z) == Material::water ||
		level->getMaterial(x + 1, y - 1, z) == Material::water ||
		level->getMaterial(x, y - 1, z - 1) == Material::water ||
		level->getMaterial(x, y - 1, z + 1) == Material::water;
}

bool ReedTile::canSurvive(Level* level, int x, int y, int z)
{
	return mayPlace(level, x, y, z);
}

void ReedTile::checkAlive(Level* level, int x, int y, int z)
{
	if (!canSurvive(level, x, y, z))
	{
		spawnResources(level, x, y, z, level->getData(x, y, z));
		level->setTile(x, y, z, TILE_AIR);
	}
}

void ReedTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	return checkAlive(level, x, y, z);
}

void ReedTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (!level->isEmptyTile(x, y, z))
		return;

	int height;
	for (height = 1; level->getTile(x, y - height, z) == m_ID; height++);

	if (height <= 2)
	{
		int data = level->getData(x, y, z);

		if (data == 15)
		{
			level->setTile(x, y + 1, z, m_ID);
			level->setData(x, y, z, 0);
		}
		else
		{
			level->setData(x, y, z, data + 1);
		}
	}
}

AABB* ReedTile::getAABB(Level* level, int x, int y, int z)
{
	return nullptr;
}

int ReedTile::getResource(int x, Random* random)
{
	return 0;
}
