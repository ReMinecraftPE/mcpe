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

bool StoneSlabTile::isSolidRender() const
{
	return m_bFull;
}

bool StoneSlabTile::isCubeShaped() const
{
	return m_bFull;
}

int StoneSlabTile::getResource(TileData data, Random* random) const
{
	return Tile::stoneSlabHalf->m_ID;
}

int StoneSlabTile::getResourceCount(Random* random) const
{
	if (m_bFull) return 2;
	return 1;
}

int StoneSlabTile::getSpawnResourcesAuxValue(int x) const
{
	return x;
}

int StoneSlabTile::getTexture(Facing::Name face) const
{
	return getTexture(face, 0);
}

int StoneSlabTile::getTexture(Facing::Name face, TileData data) const
{
	switch (data)
	{
		case STONE:
			if (face > Facing::UP)
				return TEXTURE_STONE_SLAB_SIDE;
			
			return TEXTURE_STONE_SLAB_TOP;
		case SAND:
			if (face == Facing::DOWN)
				return TEXTURE_SANDSTONE_BOTTOM;
			if (face == Facing::UP)
				return TEXTURE_SANDSTONE_TOP;

			return TEXTURE_SANDSTONE_SIDE;
		case WOOD:
			return TEXTURE_PLANKS;
		case COBBLE:
			return TEXTURE_STONEBRICK;
		default:
			return TEXTURE_STONE_SLAB_TOP;
	}
}

void StoneSlabTile::onPlace(Level* level, const TilePos& pos)
{
	TileID tileBelow = level->getTile(pos.below());
	int tileHereData = level->getData(pos);

	// If there's a stone slab below us, set us to air and combine us into the lower slab block

	if (tileHereData == level->getData(pos.below()) && tileBelow == Tile::stoneSlabHalf->m_ID)
	{
		level->setTile(pos, TILE_AIR);
		level->setTileAndData(pos.below(), Tile::stoneSlab->m_ID, tileHereData);
	}
}

bool StoneSlabTile::shouldRenderFace(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	if (this != Tile::stoneSlabHalf)
		// @BUG: Missing return? In JE this is true too
		Tile::shouldRenderFace(level, pos, face);

	if (face == Facing::UP)
		return true;

	if (!Tile::shouldRenderFace(level, pos, face))
		return false;

	if (face == Facing::DOWN)
		return true;

	return level->getTile(pos) != m_ID;
}
