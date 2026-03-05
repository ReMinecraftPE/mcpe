/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FenceTile.hpp"
#include "world/level/TileSource.hpp"

FenceTile::FenceTile(int a, int b) : Tile(a, b, Material::wood)
{
}

bool FenceTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	TilePos below = pos.below();
	return source->getTile(below) == m_ID || (Tile::mayPlace(source, pos) && source->getMaterial(below)->isSolid());
}

AABB* FenceTile::getAABB(TileSource* source, const TilePos& pos)
{
	AABB* rAABB = Tile::getAABB(source, pos);
	rAABB->max.y += 0.5f;
	return rAABB;
}

bool FenceTile::isSolidRender() const
{
	return false;
}

bool FenceTile::isCubeShaped() const
{
	return false;
}

eRenderShape FenceTile::getRenderShape() const
{
	return SHAPE_FENCE;
}
