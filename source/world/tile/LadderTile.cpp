/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LadderTile.hpp"
#include "world/level/Level.hpp"

LadderTile::LadderTile(int ID, int texture) : Tile(ID, texture, Material::decoration)
{
}

int LadderTile::getRenderShape() const
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

AABB* LadderTile::getAABB(const Level* level, const TilePos& pos)
{
	TileData data = level->getData(pos);
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

	return Tile::getAABB(level, pos);
}

AABB LadderTile::getTileAABB(const Level* level, const TilePos& pos)
{
	TileData data = level->getData(pos);
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

	return Tile::getTileAABB(level, pos);
}

void LadderTile::setPlacedOnFace(Level* level, const TilePos& pos, Facing::Name face)
{
	TileData data = level->getData(pos);

	if ((data == 0 || face == Facing::NORTH) && level->isSolidTile(pos.south())) data = 2;
	if ((data == 0 || face == Facing::SOUTH) && level->isSolidTile(pos.north())) data = 3;
	if ((data == 0 || face == Facing::WEST) && level->isSolidTile(pos.east())) data = 4;
	if ((data == 0 || face == Facing::EAST) && level->isSolidTile(pos.west())) data = 5;

	level->setData(pos, data);
	assert(level->getData(pos) == data);
}

void LadderTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	TileData data = level->getData(pos);
	switch (data)
	{
		case 2:
			if (level->isSolidTile(pos.south())) return;
			break;
		case 3:
			if (level->isSolidTile(pos.north())) return;
			break;
		case 4:
			if (level->isSolidTile(pos.east())) return;
			break;
		case 5:
			if (level->isSolidTile(pos.west())) return;
			break;
		case 0:
			// hasn't decided on anything right now?
			break;
		default:
			// invalid state
			assert(!"invalid ladder state");
			break;
	}

	spawnResources(level, pos, data);
	level->setTile(pos, TILE_AIR);
}

bool LadderTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return
		level->isSolidTile(pos.west()) ||
		level->isSolidTile(pos.east()) ||
		level->isSolidTile(pos.north()) ||
		level->isSolidTile(pos.south());
}
