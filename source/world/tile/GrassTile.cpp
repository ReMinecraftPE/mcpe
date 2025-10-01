/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GrassTile.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"

GrassTile::GrassTile(TileID id, Material* c) : Tile(id, c)
{
	m_TextureFrame = TEXTURE_GRASS_SIDE;
	setTicking(true);
}

int GrassTile::getColor(const LevelSource* levelSource, const TilePos& pos) const
{
	if (GetPatchManager()->IsGrassTinted())
	{
		return 0x339933;
	}

	return 0xffffff;
}

int GrassTile::getResource(TileData data, Random* random) const
{
	return Tile::dirt->getResource(data, random);
}

int GrassTile::getTexture(Facing::Name face) const
{
	switch (face)
	{
	case Facing::UP:
		return TEXTURE_GRASS_TOP;
	case Facing::DOWN:
		return TEXTURE_DIRT;
	default:
		return TEXTURE_GRASS_SIDE;
	}
}

int GrassTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	switch (face)
	{
	case Facing::UP:
		return TEXTURE_GRASS_TOP;
	case Facing::DOWN:
		return TEXTURE_DIRT;
	}

	Material* pMat = level->getMaterial(pos.above());
	if (pMat == Material::topSnow || pMat == Material::snow)
		return TEXTURE_GRASS_SIDE_SNOW;

	return TEXTURE_GRASS_SIDE;
}

void GrassTile::tick(Level* level, const TilePos& pos, Random* random)
{
	// Controls the spread/death of grass.
	// It's like a full on automata of sorts. :)
	if (level->m_bIsClientSide)
		return;

	if (level->getRawBrightness(pos.above()) <= 3 &&
		level->getMaterial(pos.above())->blocksLight())
	{
		// grass death
		if (random->genrand_int32() % 4 == 0)
			level->setTile(pos, Tile::dirt->m_ID);
	}
	else if (level->getRawBrightness(pos.above()) > 8)
	{
		TilePos tp(pos.x - 1 + random->nextInt(3),
		           pos.y - 3 + random->nextInt(5),
		           pos.z - 1 + random->nextInt(3));

		if (level->getTile(tp) == Tile::dirt->m_ID &&
			level->getRawBrightness(tp.above()) > 3 &&
			!level->getMaterial(tp.above())->blocksLight())
		{
			//@NOTE: not this->m_ID
			level->setTile(tp, Tile::grass->m_ID);
		}
	}
}
