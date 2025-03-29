/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SandTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/FallingTile.hpp"

//@NOTE: True for now
bool SandTile::instaFall = true;

SandTile::SandTile(int ID, int texture, Material* pMtl) : Tile(ID, texture, pMtl)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	// the reason sand doesn't fall in the original MCPE:
	setTicking(true);
#endif
}

int SandTile::getTickDelay() const
{
	return 3; // 3 on Java, 2 on PE, no idea why
}

void SandTile::checkSlide(Level* level, const TilePos& pos)
{
	//TileID tile = level->getTile(pos.below());

	if (!isFree(level, pos.below()))
		// standing on something, don't fall
		return;

	if (pos.y < 0)
		return;

	if (SandTile::instaFall || !level->hasChunksAt(pos, 32))
	{
		level->setTile(pos, 0);

		int y2;
		for (y2 = pos.y - 1; y2 >= 0; y2--)
		{
			if (!isFree(level, TilePos(pos.x, y2, pos.z)))
				break;
		}

		if (y2 > -1)
			level->setTile(TilePos(pos.x, y2 + 1, pos.z), m_ID);
	}
	else
	{
		// The original code attempts to spawn a falling tile entity, but it fails since it's not a player.
		// The falling sand tile
#if defined(ORIGINAL_CODE) || defined(ENH_ALLOW_SAND_GRAVITY)
		bool isEmpty = level->isEmptyTile(pos); // from 0.7.0 in HeavyTile
		Entity* fallingTile = new FallingTile(level, Vec3(pos) + 0.5f, m_ID, isEmpty);
		//setTicking(true); // from 0.7.0, for HeavyTile
		level->addEntity(fallingTile);
		LOG_I("%d: Added FallingTile entity", level->getTime());
#endif
	}
}

bool SandTile::isFree(Level* level, const TilePos& pos)
{
	TileID tile = level->getTile(pos);
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

void SandTile::tick(Level* level, const TilePos& pos, Random* random)
{
	// Specific to PE
	if (level->m_bIsMultiplayer)
		return;

	checkSlide(level, pos);
}

void SandTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	level->addToTickNextTick(pos, m_ID, getTickDelay());
#endif
}

void SandTile::onPlace(Level* level, const TilePos& pos)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	level->addToTickNextTick(pos, m_ID, getTickDelay());
#endif
}

