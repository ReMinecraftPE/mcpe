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

int LadderTile::getRenderShape()
{
	return SHAPE_LADDER;
}

int LadderTile::getResourceCount(Random* random)
{
	return 1;
}

bool LadderTile::isCubeShaped()
{
	return false;
}

bool LadderTile::isSolidRender()
{
	return false;
}

AABB* LadderTile::getAABB(Level* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
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

	return Tile::getAABB(level, x, y, z);
}

AABB LadderTile::getTileAABB(Level* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
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

	return Tile::getTileAABB(level, x, y, z);
}

void LadderTile::setPlacedOnFace(Level* level, int x, int y, int z, int face)
{
	int data = level->getData(x, y, z);

	if ((data == 0 || face == DIR_ZNEG) && level->isSolidTile(x, y, z + 1)) data = 2;
	if ((data == 0 || face == DIR_ZPOS) && level->isSolidTile(x, y, z - 1)) data = 3;
	if ((data == 0 || face == DIR_XNEG) && level->isSolidTile(x + 1, y, z)) data = 4;
	if ((data == 0 || face == DIR_XPOS) && level->isSolidTile(x - 1, y, z)) data = 5;

	level->setData(x, y, z, data);
	assert(level->getData(x, y, z) == data);
}

void LadderTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	int data = level->getData(x, y, z);
	switch (data)
	{
		case 2:
			if (level->isSolidTile(x, y, z + 1)) return;
			break;
		case 3:
			if (level->isSolidTile(x, y, z - 1)) return;
			break;
		case 4:
			if (level->isSolidTile(x + 1, y, z)) return;
			break;
		case 5:
			if (level->isSolidTile(x - 1, y, z)) return;
			break;
		case 0:
			// hasn't decided on anything right now?
			break;
		default:
			// invalid state
			assert(!"invalid ladder state");
			break;
	}

	spawnResources(level, x, y, z, data);
	level->setTile(x, y, z, TILE_AIR);
}

bool LadderTile::mayPlace(Level* level, int x, int y, int z)
{
	return
		level->isSolidTile(x - 1, y, z) ||
		level->isSolidTile(x + 1, y, z) ||
		level->isSolidTile(x, y, z - 1) ||
		level->isSolidTile(x, y, z + 1);
}
