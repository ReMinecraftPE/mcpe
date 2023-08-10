/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TorchTile.hpp"
#include "world/level/Level.hpp"

TorchTile::TorchTile(int ID, int texture, Material* pMtl) : Tile(ID, texture, pMtl)
{
	setTicking(true);
}

AABB* TorchTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

int TorchTile::getRenderShape()
{
	return SHAPE_TORCH;
}

bool TorchTile::isCubeShaped()
{
	return false;
}

bool TorchTile::isSolidRender()
{
	return false;
}

void TorchTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	float partX = float(x) + 0.5f, partZ = float(z) + 0.5f, partY = float(y) + 0.7f;

	// @NOTE: Need to use addParticle("smoke") 5 times. Invalid data values don't actually generate a smoke
	switch (level->getData(x, y, z))
	{
		case 1:
			partX -= 0.27f;
			partY += 0.22f;
			level->addParticle("smoke", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
			break;
		case 2:
			partX += 0.27f;
			partY += 0.22f;
			level->addParticle("smoke", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
			break;
		case 3:
			partZ -= 0.27f;
			partY += 0.22f;
			level->addParticle("smoke", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
			break;
		case 4:
			partZ += 0.27f;
			partY += 0.22f;
			level->addParticle("smoke", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
			break;
		case 5:
			level->addParticle("smoke", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
			break;
	}

	level->addParticle("flame", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
}

bool TorchTile::checkCanSurvive(Level* level, int x, int y, int z)
{
	if (mayPlace(level, x, y, z))
		return true;

	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);

	return false;
}

HitResult TorchTile::clip(Level* level, int x, int y, int z, Vec3 a, Vec3 b)
{
	switch (level->getData(x, y, z) &7)
	{
		case 1:
			setShape(0.0f, 0.2f, 0.35f, 0.3f, 0.8f, 0.65f);
			break;
		case 2:
			setShape(0.7f, 0.2f, 0.35f, 1.0f, 0.8f, 0.65f);
			break;
		case 3:
			setShape(0.35f, 0.2f, 0.0f, 0.65f, 0.8f, 0.3f);
			break;
		case 4:
			setShape(0.35f, 0.2f, 0.7f, 0.65f, 0.8f, 1.0f);
			break;
		default:
			setShape(0.4f, 0.0f, 0.4f, 0.6f, 0.6f, 0.6f);
			break;
	}

	return Tile::clip(level, x, y, z, a, b);
}

bool TorchTile::mayPlace(Level* level, int x, int y, int z)
{
	if (level->isSolidTile(x, y - 1, z)) return true;
	if (level->isSolidTile(x - 1, y, z)) return true;
	if (level->isSolidTile(x + 1, y, z)) return true;
	if (level->isSolidTile(x, y, z - 1)) return true;
	if (level->isSolidTile(x, y, z + 1)) return true;

	return false;
}

void TorchTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (!checkCanSurvive(level, x, y, z))
		return;

	int data = level->getData(x, y, z);

	bool flag = false;
	if (!level->isSolidTile(x - 1, y, z) && data == 1) flag = true;
	if (!level->isSolidTile(x + 1, y, z) && data == 2) flag = true;
	if (!level->isSolidTile(x, y, z - 1) && data == 3) flag = true;
	if (!level->isSolidTile(x, y, z + 1) && data == 4) flag = true;
	if (!level->isSolidTile(x, y - 1, z) && data == 5) flag = true;

	if (!flag)
		return; // all good
	
	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
}

void TorchTile::onPlace(Level* level, int x, int y, int z)
{
	int data = 0;

	if (level->isSolidTile(x - 1, y, z))
		level->setData(x, y, z, 1);
	else if (level->isSolidTile(x + 1, y, z))
		level->setData(x, y, z, 2);
	else if (level->isSolidTile(x, y, z - 1))
		level->setData(x, y, z, 3);
	else if (level->isSolidTile(x, y, z + 1))
		level->setData(x, y, z, 4);
	else if (level->isSolidTile(x, y - 1, z))
		level->setData(x, y, z, 5);

	checkCanSurvive(level, x, y, z);
}

void TorchTile::setPlacedOnFace(Level* level, int x, int y, int z, int dir)
{
	int data = level->getData(x, y, z);

	switch (dir)
	{
		case DIR_YPOS:
			if (level->isSolidTile(x, y - 1, z))
				data = 5;
			break;
		case DIR_ZNEG:
			if (level->isSolidTile(x, y, z + 1))
				data = 4;
			break;
		case DIR_ZPOS:
			if (level->isSolidTile(x, y, z - 1))
				data = 3;
			break;
		case DIR_XNEG:
			if (level->isSolidTile(x + 1, y, z))
				data = 2;
			break;
		case DIR_XPOS:
			if (level->isSolidTile(x - 1, y, z))
				data = 1;
			break;
	}

	level->setData(x, y, z, data);
}

void TorchTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (!level->getData(x, y, z))
		onPlace(level, x, y, z);
}
