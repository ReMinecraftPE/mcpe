/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GrassTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "client/renderer/PatchManager.hpp"

const Color GrassTile::DEFAULT_COLOR = Color(0.25f, 0.60f, 0.25f);

GrassTile::GrassTile(TileID id, Material* c) : Tile(id, c)
{
	m_TextureFrame = TEXTURE_GRASS_SIDE;
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setTicking(true);
}

int GrassTile::getColor(TileSource* source, const TilePos& pos) const
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

int GrassTile::getTexture(TileSource* source, const TilePos& pos, Facing::Name face) const
{
	switch (face)
	{
	case Facing::UP:
		return TEXTURE_GRASS_TOP;
	case Facing::DOWN:
		return TEXTURE_DIRT;
	default:
		break;
	}

	Material* pMat = source->getMaterial(pos.above());
	if (pMat == Material::topSnow || pMat == Material::snow)
		return TEXTURE_GRASS_SIDE_SNOW;

	return TEXTURE_GRASS_SIDE;
}

void GrassTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	// Controls the spread/death of grass.
	// It's like a full on automata of sorts. :)
	if (source->getLevelConst().m_bIsClientSide)
		return;

	if (source->getRawBrightness(pos.above()) <= 3 &&
		source->getMaterial(pos.above())->blocksLight())
	{
		// grass death
		if (random->genrand_int32() % 4 == 0)
			source->setTile(pos, Tile::dirt->m_ID);
	}
	else if (source->getRawBrightness(pos.above()) > 8)
	{
		TilePos tp(pos.x - 1 + random->nextInt(3),
		           pos.y - 3 + random->nextInt(5),
		           pos.z - 1 + random->nextInt(3));

		if (source->getTile(tp) == Tile::dirt->m_ID &&
			source->getRawBrightness(tp.above()) > 3 &&
			!source->getMaterial(tp.above())->blocksLight())
		{
			//@NOTE: not this->m_ID
			source->setTile(tp, Tile::grass->m_ID);
		}
	}
}
