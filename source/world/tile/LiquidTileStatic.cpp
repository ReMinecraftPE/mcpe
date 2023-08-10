/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTileStatic.hpp"
#include "world/level/Level.hpp"

LiquidTileStatic::LiquidTileStatic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
	setTicking(false);
	if (pMtl == Material::lava)
		setTicking(true);
}

bool LiquidTileStatic::isFlammable(Level* level, int x, int y, int z)
{
	return level->getMaterial(x, y, z)->isFlammable();
}

void LiquidTileStatic::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	updateLiquid(level, x, y, z);
	if (level->getTile(x, y, z) == m_ID)
		setDynamic(level, x, y, z);
}

void LiquidTileStatic::setDynamic(Level* level, int x, int y, int z)
{
	level->field_30 = true;
	level->setTileAndDataNoUpdate(x, y, z, m_ID - 1, level->getData(x, y, z));
	level->setTilesDirty(x, y, z, x, y, z);
	level->addToTickNextTick(x, y, z, m_ID - 1, getTickDelay());
	level->field_30 = false;
}

void LiquidTileStatic::tick(Level* level, int x, int y, int z, Random* random)
{
	if (m_pMaterial != Material::lava)
		return;

	int y2 = y + random->genrand_int32() % 3;
	while (y != y2)
	{
		x += random->genrand_int32() % 3 - 1;
		z += random->genrand_int32() % 3 - 1;

		TileID tile = level->getTile(x, y + 1, z);
		if (tile)
		{
			if (tiles[tile]->m_pMaterial->blocksMotion())
				return;
		}
		else if (
			isFlammable(level, x - 1, y + 1, z) ||
			isFlammable(level, x + 1, y + 1, z) ||
			isFlammable(level, x, y + 1, z - 1) ||
			isFlammable(level, x, y + 1, z + 1) ||
			isFlammable(level, x, y, z) ||
			isFlammable(level, x, y + 2, z))
		{
			level->setTile(x, y + 1, z, Tile::fire->m_ID);
			return;
		}

		y++;
	}
}
