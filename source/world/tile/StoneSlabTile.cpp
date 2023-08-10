/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StoneSlabTile.hpp"
#include "world/level/Level.hpp"

StoneSlabTile::StoneSlabTile(int id, bool full) : Tile(id, TEXTURE_STONE_SLAB_TOP, Material::stone)
{
	m_bFull = full;

	if (!full)
		setShape(0, 0, 0, 1, 0.5f, 1);

	setLightBlock(255);
}

bool StoneSlabTile::isSolidRender()
{
	return m_bFull;
}

bool StoneSlabTile::isCubeShaped()
{
	return m_bFull;
}

int StoneSlabTile::getResource(int data, Random* random)
{
	return Tile::stoneSlabHalf->m_ID;
}

int StoneSlabTile::getResourceCount(Random* random)
{
	if (m_bFull) return 2;
	return 1;
}

int StoneSlabTile::getSpawnResourcesAuxValue(int x)
{
	return x;
}

int StoneSlabTile::getTexture(int dir)
{
	return getTexture(dir, 0);
}

int StoneSlabTile::getTexture(int dir, int data)
{
	switch (data)
	{
		// regular stone slab
		case 0:
			if (dir > DIR_YPOS)
				return TEXTURE_STONE_SLAB_SIDE;
			
			return TEXTURE_STONE_SLAB_TOP;

		// sandstone slab
		case 1:
			if (dir == DIR_YNEG)
				return TEXTURE_SANDSTONE_BOTTOM;
			if (dir == DIR_YPOS)
				return TEXTURE_SANDSTONE_TOP;

			return TEXTURE_SANDSTONE_SIDE;

		// wood slab
		case 2:
			return TEXTURE_PLANKS;

		// stone brick slab
		case 3:
			return TEXTURE_STONEBRICK;

		// unknown slab type
		default:
			return TEXTURE_STONE_SLAB_TOP;
	}
}

void StoneSlabTile::onPlace(Level* level, int x, int y, int z)
{
	TileID tileBelow = level->getTile(x, y - 1, z);
	int tileHereData = level->getData(x, y, z);

	// If there's a stone slab below us, set us to air and combine us into the lower slab block

	if (tileHereData == level->getData(x, y - 1, z) && tileBelow == Tile::stoneSlabHalf->m_ID)
	{
		level->setTile(x, y, z, TILE_AIR);
		level->setTileAndData(x, y - 1, z, Tile::stoneSlab->m_ID, tileHereData);
	}
}

bool StoneSlabTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int dir)
{
	if (this != Tile::stoneSlabHalf)
		// @BUG: Missing return? In JE this is true too
		Tile::shouldRenderFace(level, x, y, z, dir);

	if (dir == DIR_YPOS)
		return true;

	if (!Tile::shouldRenderFace(level, x, y, z, dir))
		return false;

	if (dir == DIR_YNEG)
		return true;

	return level->getTile(x, y, z) != m_ID;
}
