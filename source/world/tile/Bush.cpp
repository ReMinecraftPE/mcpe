/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Bush.hpp"
#include "world/level/TileSource.hpp"

Bush::Bush(TileID id, int texture) : Tile(id, Material::plant)
{
	m_TextureFrame = texture;
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setTicking(true);
	setShape(0.3f, 0.0f, 0.3f, 0.7f, 0.6f, 0.7f);
}

eRenderShape Bush::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool Bush::isCubeShaped() const
{
	return false;
}

bool Bush::isSolidRender() const
{
	return false;
}

bool Bush::mayPlace(TileSource* source, const TilePos& pos) const
{
	TileID tile = source->getTile(pos.below());

	return tile == Tile::grass->m_ID || tile == Tile::dirt->m_ID || tile == Tile::farmland->m_ID;
}

bool Bush::canSurvive(TileSource* source, const TilePos& pos) const
{
	if (source->getRawBrightness(pos) <= 7 && !source->canSeeSky(pos))
		return false;

	return mayPlace(source, pos);
}

void Bush::checkAlive(TileSource* source, const TilePos& pos)
{
	if (!canSurvive(source, pos))
	{
		spawnResources(source, pos, source->getData(pos));
		source->setTile(pos, TILE_AIR);
	}
}

void Bush::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	return checkAlive(source, pos);
}

void Bush::tick(TileSource* source, const TilePos& pos, Random* random)
{
	checkAlive(source, pos);
}

AABB* Bush::getAABB(TileSource* source, const TilePos& pos)
{
	return nullptr;
}
