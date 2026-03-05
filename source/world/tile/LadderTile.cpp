/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LadderTile.hpp"
#include "world/level/TileSource.hpp"

LadderTile::LadderTile(int ID, int texture) : Tile(ID, texture, Material::decoration)
{
	m_renderLayer = RENDER_LAYER_ALPHATEST;
}

eRenderShape LadderTile::getRenderShape() const
{
	return SHAPE_LADDER;
}

int LadderTile::getResourceCount(Random* random) const
{
	return 1;
}

bool LadderTile::isCubeShaped() const
{
	return false;
}

bool LadderTile::isSolidRender() const
{
	return false;
}

AABB* LadderTile::getAABB(TileSource* source, const TilePos& pos)
{
	TileData data = source->getData(pos);
	switch (data)
	{
		case 2:
			setShape(0.0f, 0.0f, 0.875, 1.0f, 1.0f, 1.0f);
			break;
		case 3:
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.125f);
			break;
		case 4:
			setShape(0.875, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 5:
			setShape(0.0f, 0.0f, 0.0f, 0.125, 1.0f, 1.0f);
			break;
	}

	return Tile::getAABB(source, pos);
}

AABB LadderTile::getTileAABB(TileSource* source, const TilePos& pos)
{
	TileData data = source->getData(pos);
	switch (data)
	{
		case 2:
			setShape(0.0f, 0.0f, 0.875, 1.0f, 1.0f, 1.0f);
			break;
		case 3:
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.125f);
			break;
		case 4:
			setShape(0.875, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 5:
			setShape(0.0f, 0.0f, 0.0f, 0.125, 1.0f, 1.0f);
			break;
	}

	return Tile::getTileAABB(source, pos);
}

void LadderTile::setPlacedOnFace(TileSource* source, const TilePos& pos, Facing::Name face)
{
	TileData data = source->getData(pos);

	if ((data == 0 || face == Facing::NORTH) && source->isSolidBlockingTile(pos.south())) data = 2;
	if ((data == 0 || face == Facing::SOUTH) && source->isSolidBlockingTile(pos.north())) data = 3;
	if ((data == 0 || face == Facing::WEST) && source->isSolidBlockingTile(pos.east())) data = 4;
	if ((data == 0 || face == Facing::EAST) && source->isSolidBlockingTile(pos.west())) data = 5;

	source->setTileAndData(pos, FullTile(m_ID, data));
	assert(source->getData(pos) == data);
}

void LadderTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	TileData data = source->getData(pos);
	switch (data)
	{
		case 2:
			if (source->isSolidBlockingTile(pos.south())) return;
			break;
		case 3:
			if (source->isSolidBlockingTile(pos.north())) return;
			break;
		case 4:
			if (source->isSolidBlockingTile(pos.east())) return;
			break;
		case 5:
			if (source->isSolidBlockingTile(pos.west())) return;
			break;
		case 0:
			// hasn't decided on anything right now?
			break;
		default:
			// invalid state
			assert(!"invalid ladder state");
			break;
	}

	spawnResources(source, pos, data);
	source->setTile(pos, TILE_AIR);
}

bool LadderTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	return
		source->isSolidBlockingTile(pos.west()) ||
		source->isSolidBlockingTile(pos.east()) ||
		source->isSolidBlockingTile(pos.north()) ||
		source->isSolidBlockingTile(pos.south());
}
