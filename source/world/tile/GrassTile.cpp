/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

GrassTile::GrassTile(int id, Material* c) : Tile(id, c)
{
	m_TextureFrame = TEXTURE_GRASS_SIDE;
	setTicking(true);
}

int GrassTile::getColor(LevelSource*, int x, int y, int z)
{
#ifdef MOD_DONT_COLOR_GRASS
	return 0xffffff;
#endif
	return 0x339933;
}

int GrassTile::getResource(int i, Random* random)
{
	return Tile::dirt->getResource(i, random);
}

int GrassTile::getTexture(LevelSource* level, int x, int y, int z, int dir)
{
	if (dir == DIR_YPOS)
		return TEXTURE_GRASS_TOP;

	if (dir == DIR_YNEG)
		return TEXTURE_DIRT;

	Material* pMat = level->getMaterial(x, y + 1, z);
	if (pMat == Material::topSnow || pMat == Material::snow)
		return TEXTURE_GRASS_SIDE_SNOW;

	return TEXTURE_GRASS_SIDE;
}

void GrassTile::tick(Level* level, int x, int y, int z, Random* random)
{
	// Controls the spread/death of grass.
	// It's like a full on automata of sorts. :)
	if (level->field_11)
		return;

	if (level->getRawBrightness(x, y + 1, z) <= 3 &&
		level->getMaterial(x, y + 1, z)->blocksLight())
	{
		// grass death
		if (random->genrand_int32() % 4 == 0)
			level->setTile(x, y, z, Tile::dirt->m_ID);
	}
	else if (level->getRawBrightness(x, y + 1, z) > 8)
	{
		int newX = x - 1 + random->nextInt(3);
		int newY = y - 3 + random->nextInt(5);
		int newZ = z - 1 + random->nextInt(3);

		if (level->getTile(newX, newY, newZ) == Tile::dirt->m_ID &&
			level->getRawBrightness(newX, newY + 1, newZ) > 3 &&
			!level->getMaterial(newX, newY + 1, newZ)->blocksLight())
		{
			//@NOTE: not this->m_ID
			level->setTile(newX, newY, newZ, Tile::grass->m_ID);
		}
	}
}
