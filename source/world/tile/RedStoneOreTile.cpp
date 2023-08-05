/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

RedStoneOreTile::RedStoneOreTile(int id, int texture, bool bLit) : Tile(id, texture, Material::stone)
{
	m_bLit = bLit;
	if (bLit)
		setTicking(true);
}

int RedStoneOreTile::getResource(int x, Random* random)
{
	return 0;
}

int RedStoneOreTile::getResourceCount(Random* random)
{
	return random->genrand_int32() % 2 + 4;
}

int RedStoneOreTile::getSpawnResourcesAuxValue(int x)
{
	return 0;
}

int RedStoneOreTile::poofParticles(Level* level, int x, int y, int z)
{
	for (int i = 0; i < 6; i++)
	{
		float xo = float(x) + level->field_38.nextFloat();
		float yo = float(y) + level->field_38.nextFloat();
		float zo = float(z) + level->field_38.nextFloat();

		switch (i)
		{
			case DIR_YNEG:
				if (!level->isSolidTile(x, y + 1, z))
					yo = float(y + 1) + 0.0625f;
				break;
			case DIR_YPOS:
				if (!level->isSolidTile(x, y - 1, z))
					yo = float(y) - 0.0625f;
				break;
			case DIR_ZNEG:
				if (!level->isSolidTile(x, y, z + 1))
					zo = float(z + 1) + 0.0625f;
				break;
			case DIR_ZPOS:
				if (!level->isSolidTile(x, y, z - 1))
					zo = float(z) - 0.0625f;
				break;
			case DIR_XNEG:
				if (!level->isSolidTile(x + 1, y, z))
					xo = float(x + 1) + 0.0625f;
				break;
			case DIR_XPOS:
				if (!level->isSolidTile(x - 1, y, z))
					xo = float(x) - 0.0625f;
				break;
		}

		if (xo < float(x) || float(x + 1) < xo || yo < 0.0f || float(y + 1) < yo || zo < float(z) || float(z + 1) < zo)
		{
			level->addParticle("reddust", xo, yo, zo, 0.0f, 0.0f, 0.0f);
		}
	}

	// @HUH: For whatever reason??
	return 1300;
}

void RedStoneOreTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	if (m_bLit)
		poofParticles(level, x, y, z);
}

void RedStoneOreTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (m_ID == Tile::redStoneOre_lit->m_ID)
		level->setTile(x, y, z, Tile::redStoneOre->m_ID);
}

void RedStoneOreTile::interact(Level* level, int x, int y, int z)
{
	poofParticles(level, x, y, z);

	if (m_ID == Tile::redStoneOre->m_ID)
		level->setTile(x, y, z, Tile::redStoneOre_lit->m_ID);
}

void RedStoneOreTile::attack(Level* level, int x, int y, int z, Player* player)
{
	interact(level, x, y, z);
}

int RedStoneOreTile::use(Level* level, int x, int y, int z, Player* player)
{
	interact(level, x, y, z);
	return Tile::use(level, x, y, z, player);
}

void RedStoneOreTile::stepOn(Level* level, int x, int y, int z, Entity* entity)
{
	interact(level, x, y, z);
}

int RedStoneOreTile::getTickDelay()
{
	return 30;
}
