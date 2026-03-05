/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SandTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
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
	return 3;
}

void SandTile::checkSlide(TileSource* source, const TilePos& pos)
{
	//TileID tile = level->getTile(pos.below());

	if (!isFree(source, pos.below()))
		// standing on something, don't fall
		return;

	if (pos.y <= 0)
		return;

	if (SandTile::instaFall || !source->hasChunksAt(TilePos(pos.x - 32, pos.y - 32, pos.z - 32), TilePos(pos.x + 32, pos.y + 32, pos.z + 32)))
	{
		source->setTile(pos, TILE_AIR);

		int y2;
		for (y2 = pos.y - 1; y2 >= 0; y2--)
		{
			if (!isFree(source, TilePos(pos.x, y2, pos.z)))
				break;
		}

		if (y2 > -1)
			source->setTile(TilePos(pos.x, y2 + 1, pos.z), m_ID);
	}
	else
	{
		// The original code attempts to spawn a falling tile entity, but it fails since it's not a player.
		// The falling sand tile
#if defined(ORIGINAL_CODE) || defined(ENH_ALLOW_SAND_GRAVITY)
		Level& level = source->getLevel();
		level.addEntity(new FallingTile(*source, Vec3(float(pos.x) + 0.5f, float(pos.y) + 0.5f, float(pos.z) + 0.5f), m_ID));
#endif
	}
}

bool SandTile::isFree(TileSource* source, const TilePos& pos)
{
	TileID tile = source->getTile(pos);
	if (tile == TILE_AIR)
		return true;

	if (tile == Tile::fire->m_ID)
		return true;

	if (Tile::tiles[tile]->m_pMaterial == Material::water)
		return true;

	if (Tile::tiles[tile]->m_pMaterial == Material::lava)
		return true;

	return false;
}

void SandTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getLevelConst().m_bIsClientSide)
		return;

	checkSlide(source, pos);
}

void SandTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
#endif
}

void SandTile::onPlace(TileSource* source, const TilePos& pos)
{
#ifdef ENH_ALLOW_SAND_GRAVITY
	source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
#endif
}

