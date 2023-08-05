/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

FarmTile::FarmTile(int a, Material* c) : Tile(a, c)
{
	m_TextureFrame = TEXTURE_FARMLAND_DRY;

	setTicking(true);
	setShape(0, 0, 0, 1, 15.0f / 16.0f, 1.0f);
	setLightBlock(255);
}

AABB* FarmTile::getAABB(Level*, int x, int y, int z)
{
	// a full block
	m_aabbReturned = AABB(
		float(x),
		float(y),
		float(z),
		float(x + 1),
		float(y + 1),
		float(z + 1)
	);

	return &m_aabbReturned;
}

int FarmTile::getResource(int x, Random* random)
{
	return Tile::dirt->getResource(x, random);
}

int FarmTile::getTexture(int dir, int data)
{
	if (dir == DIR_YPOS)
	{
		if (data > 0)
			return m_TextureFrame - 1;

		return m_TextureFrame;
	}

	return TEXTURE_DIRT;
}

bool FarmTile::isCubeShaped()
{
	return false;
}

bool FarmTile::isSolidRender()
{
	return false;
}

bool FarmTile::isNearWater(Level* level, int x, int y, int z)
{
	for (int ax = x - 4; ax <= x + 4; ax++)
	{
		for (int ay = y; ay <= y + 1; ay++)
		{
			for (int az = z - 4; az <= z + 4; az++)
			{
				if (level->getMaterial(ax, ay, az) == Material::water)
					return true;
			}
		}
	}

	return false;
}

void FarmTile::neighborChanged(Level* level, int x, int y, int z, int a)
{
	if (level->getMaterial(x, y + 1, z)->isSolid())
		level->setTile(x, y, z, Tile::dirt->m_ID);
}

void FarmTile::stepOn(Level* level, int x, int y, int z, Entity* pEnt)
{
	if (level->field_38.genrand_int32() % 4 == 0)
		level->setTile(x, y, z, Tile::dirt->m_ID);
}

void FarmTile::tick(Level* level, int x, int y, int z, Random* random)
{
	int val = random->genrand_int32();

	//@HUH: weird way of saying val % 5 == 0
	if (val != 5 * (val / 5))
		return;

	if (isNearWater(level, x, y, z))
	{
		level->setData(x, y, z, 7);
	}
	else
	{
		int data = level->getData(x, y, z);

		if (data <= 0)
			level->setTile(x, y, z, Tile::dirt->m_ID);
		else
			level->setData(x, y, z, data - 1);
	}
}
