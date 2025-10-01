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

AABB* TorchTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

int TorchTile::getRenderShape() const
{
	return SHAPE_TORCH;
}

bool TorchTile::isCubeShaped() const
{
	return false;
}

bool TorchTile::isSolidRender() const
{
	return false;
}

void TorchTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	Vec3 part(pos);
	part += 0.5f;
	part.y += 0.2f;

	// @NOTE: Need to use addParticle("smoke") 5 times. Invalid data values don't actually generate a smoke
	switch (level->getData(pos))
	{
		case 1:
			part.x -= 0.27f;
			part.y += 0.22f;
			level->addParticle("smoke", part);
			break;
		case 2:
			part.x += 0.27f;
			part.y += 0.22f;
			level->addParticle("smoke", part);
			break;
		case 3:
			part.z -= 0.27f;
			part.y += 0.22f;
			level->addParticle("smoke", part);
			break;
		case 4:
			part.z += 0.27f;
			part.y += 0.22f;
			level->addParticle("smoke", part);
			break;
		case 5:
			level->addParticle("smoke", part);
			break;
	}

	level->addParticle("flame", part);
}

bool TorchTile::checkCanSurvive(Level* level, const TilePos& pos)
{
	if (mayPlace(level, pos))
		return true;

	spawnResources(level, pos, level->getData(pos));
	level->setTile(pos, TILE_AIR);

	return false;
}

HitResult TorchTile::clip(const Level* level, const TilePos& pos, Vec3 a, Vec3 b)
{
	switch (level->getData(pos) &7)
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

	return Tile::clip(level, pos, a, b);
}

bool TorchTile::mayPlace(const Level* level, const TilePos& pos) const
{
	if (level->isSolidTile(pos.below())) return true;
	if (level->isSolidTile(pos.west())) return true;
	if (level->isSolidTile(pos.east())) return true;
	if (level->isSolidTile(pos.north())) return true;
	if (level->isSolidTile(pos.south())) return true;

	return false;
}

void TorchTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!checkCanSurvive(level, pos))
		return;

	TileData data = level->getData(pos);

	bool flag = false;
	if (!level->isSolidTile(pos.west()) && data == 1) flag = true;
	if (!level->isSolidTile(pos.east()) && data == 2) flag = true;
	if (!level->isSolidTile(pos.north()) && data == 3) flag = true;
	if (!level->isSolidTile(pos.south()) && data == 4) flag = true;
	if (!level->isSolidTile(pos.below()) && data == 5) flag = true;

	if (!flag)
		return; // all good
	
	spawnResources(level, pos, level->getData(pos));
	level->setTile(pos, TILE_AIR);
}

void TorchTile::onPlace(Level* level, const TilePos& pos)
{
	if (level->isSolidTile(pos.west()))
		level->setData(pos, 1);
	else if (level->isSolidTile(pos.east()))
		level->setData(pos, 2);
	else if (level->isSolidTile(pos.north()))
		level->setData(pos, 3);
	else if (level->isSolidTile(pos.south()))
		level->setData(pos, 4);
	else if (level->isSolidTile(pos.below()))
		level->setData(pos, 5);

	checkCanSurvive(level, pos);
}

void TorchTile::setPlacedOnFace(Level* level, const TilePos& pos, Facing::Name face)
{
	TileData data = level->getData(pos);

	switch (face)
	{
		case Facing::UP:
			if (level->isSolidTile(pos.below()))
				data = 5;
			break;
		case Facing::NORTH:
			if (level->isSolidTile(pos.south()))
				data = 4;
			break;
		case Facing::SOUTH:
			if (level->isSolidTile(pos.north()))
				data = 3;
			break;
		case Facing::WEST:
			if (level->isSolidTile(pos.east()))
				data = 2;
			break;
		case Facing::EAST:
			if (level->isSolidTile(pos.west()))
				data = 1;
			break;
	}

	level->setData(pos, data);
}

void TorchTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (!level->getData(pos))
		onPlace(level, pos);
}
