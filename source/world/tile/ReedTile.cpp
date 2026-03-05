/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ReedTile.hpp"
#include "world/level/TileSource.hpp"

ReedTile::ReedTile(TileID id) : Tile(id, Material::plant)
{
	m_TextureFrame = TEXTURE_REEDS;
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setShape(0.125f, 0.0f, 0.125f, 0.875f, 1.0f, 0.875f);
	setTicking(true);
}

eRenderShape ReedTile::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool ReedTile::isCubeShaped() const
{
	return false;
}

bool ReedTile::isSolidRender() const
{
	return false;
}

bool ReedTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	TileID tileBelow = source->getTile(pos.below());

	if (tileBelow == m_ID)
		return true;

	//@NOTE: No sand
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	return
		source->getMaterial(pos.below().west()) == Material::water ||
		source->getMaterial(pos.below().east()) == Material::water ||
		source->getMaterial(pos.below().north()) == Material::water ||
		source->getMaterial(pos.below().south()) == Material::water;
}

bool ReedTile::canSurvive(TileSource* source, const TilePos& pos) const
{
	return mayPlace(source, pos);
}

void ReedTile::checkAlive(TileSource* source, const TilePos& pos)
{
	if (!canSurvive(source, pos))
	{
		spawnResources(source, pos, source->getData(pos));
		source->setTile(pos, TILE_AIR);
	}
}

void ReedTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	return checkAlive(source, pos);
}

void ReedTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (!source->isEmptyTile(pos))
		return;

	int height;
	for (height = 1; source->getTile(TilePos(pos.x, pos.y - height, pos.z)) == m_ID; height++);

	if (height <= 2)
	{
		TileData data = source->getData(pos);

		if (data == 15)
		{
			source->setTileAndData(pos.above(), FullTile(m_ID, 0));
			source->setTileAndData(pos, FullTile(m_ID, 0));
		}
		else
		{
			source->setTileAndData(pos, FullTile(m_ID, data + 1));
		}
	}
}

AABB* ReedTile::getAABB(TileSource* source, const TilePos& pos)
{
	return nullptr;
}

int ReedTile::getResource(TileData data, Random* random) const
{
	return Item::reeds->m_itemID;
}
