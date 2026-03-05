/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TopSnowTile.hpp"
#include "world/level/TileSource.hpp"

TopSnowTile::TopSnowTile(TileID id, int b, Material* c) : Tile(id, b, c)
{
	setShape(0, 0, 0, 1, 0.125f, 1);
	setTicking(true);
}

AABB* TopSnowTile::getAABB(TileSource* source, const TilePos& pos)
{
	return nullptr;
}

bool TopSnowTile::isCubeShaped() const
{
	return false;
}

bool TopSnowTile::isSolidRender() const
{
	return false;
}

int TopSnowTile::getResource(TileData data, Random* random) const
{
	return 0;
}

int TopSnowTile::getResourceCount(Random* random) const
{
	return 0;
}

bool TopSnowTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	TileID tile = source->getTile(pos.below());

	if (!tile || !Tile::tiles[tile]->isSolidRender())
		return false;

	return source->getMaterial(pos.below())->blocksMotion();
}

bool TopSnowTile::checkCanSurvive(TileSource* source, const TilePos& pos)
{
	if (mayPlace(source, pos))
		return true;

	spawnResources(source, pos, source->getData(pos));
	source->setTile(pos, TILE_AIR);
	return false;
}

void TopSnowTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	checkCanSurvive(source, pos);
}

bool TopSnowTile::shouldRenderFace(TileSource* source, const TilePos& pos, Facing::Name face) const
{
	if (face == Facing::UP)
		return true;

	if (source->getMaterial(pos) == m_pMaterial)
		return false;

	return Tile::shouldRenderFace(source, pos, face);
}

void TopSnowTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getBrightness(LightLayer::Block, pos) > 11)
	{
		spawnResources(source, pos, source->getData(pos));
		source->setTile(pos, TILE_AIR);
	}
}
