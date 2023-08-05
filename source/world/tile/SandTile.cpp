/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"
#include "FallingTile.hpp"

//@NOTE: True for now
bool SandTile::instaFall = true;

SandTile::SandTile(int ID, int texture, Material* pMtl) : Tile(ID, texture, pMtl)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	// the reason sand doesn't fall in the original MCPE:
	setTicking(true);
#endif
}

int SandTile::getTickDelay()
{
	return 3;
}

void SandTile::checkSlide(Level* level, int x, int y, int z)
{
	TileID tile = level->getTile(x, y - 1, z);

	if (!isFree(level, x, y - 1, z))
		// standing on something, don't fall
		return;

	if (y <= 0)
		return;

	if (SandTile::instaFall || !level->hasChunksAt(x - 32, y - 32, z - 32, x + 32, y + 32, z + 32))
	{
		level->setTile(x, y, z, 0);

		int y2;
		for (y2 = y - 1; y2 >= 0; y2--)
		{
			if (!isFree(level, x, y2, z))
				break;
		}

		if (y2 > -1)
			level->setTile(x, y2 + 1, z, m_ID);
	}
	else
	{
		// The original code attempts to spawn a falling tile entity, but it fails since it's not a player.
		// The falling sand tile
#if defined(ORIGINAL_CODE) || defined(ENH_ALLOW_SAND_GRAVITY)
		level->addEntity(new FallingTile(level, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, m_ID));
#endif
	}
}

bool SandTile::isFree(Level* level, int x, int y, int z)
{
	TileID tile = level->getTile(x, y, z);
	if (!tile)
		return true;

	if (tile == Tile::fire->m_ID)
		return true;

	if (Tile::tiles[tile]->m_pMaterial == Material::water)
		return true;

	if (Tile::tiles[tile]->m_pMaterial == Material::lava)
		return true;

	return false;
}

void SandTile::tick(Level* level, int x, int y, int z, Random* random)
{
	checkSlide(level, x, y, z);
}

void SandTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
#endif
}

void SandTile::onPlace(Level* level, int x, int y, int z)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
#endif
}

