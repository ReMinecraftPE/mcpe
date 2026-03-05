/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TorchTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"

TorchTile::TorchTile(int ID, int texture, Material* pMtl) : Tile(ID, texture, pMtl)
{
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setTicking(true);
}

AABB* TorchTile::getAABB(TileSource*, const TilePos& pos)
{
	return nullptr;
}

eRenderShape TorchTile::getRenderShape() const
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

void TorchTile::animateTick(TileSource* source, const TilePos& pos, Random* random)
{
	Vec3 part(pos);
	part += 0.5f;
	part.y += 0.2f;

	// @NOTE: Need to use addParticle("smoke") 5 times. Invalid data values don't actually generate a smoke
	Level& level = source->getLevel();

	switch (source->getData(pos))
	{
		case 1:
			part.x -= 0.27f;
			part.y += 0.22f;
			level.addParticle("smoke", part);
			break;
		case 2:
			part.x += 0.27f;
			part.y += 0.22f;
			level.addParticle("smoke", part);
			break;
		case 3:
			part.z -= 0.27f;
			part.y += 0.22f;
			level.addParticle("smoke", part);
			break;
		case 4:
			part.z += 0.27f;
			part.y += 0.22f;
			level.addParticle("smoke", part);
			break;
		case 5:
			level.addParticle("smoke", part);
			break;
	}

	level.addParticle("flame", part);
}

bool TorchTile::checkCanSurvive(TileSource* source, const TilePos& pos)
{
	if (mayPlace(source, pos))
		return true;

	spawnResources(source, pos, source->getData(pos));
	source->setTile(pos, TILE_AIR);

	return false;
}

HitResult TorchTile::clip(TileSource* source, const TilePos& pos, Vec3 a, Vec3 b)
{
	switch (source->getData(pos) & 7)
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

	return Tile::clip(source, pos, a, b);
}

bool TorchTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	if (source->isSolidBlockingTile(pos.below())) return true;
	if (source->isSolidBlockingTile(pos.west())) return true;
	if (source->isSolidBlockingTile(pos.east())) return true;
	if (source->isSolidBlockingTile(pos.north())) return true;
	if (source->isSolidBlockingTile(pos.south())) return true;

	return false;
}

void TorchTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	if (!checkCanSurvive(source, pos))
		return;

	TileData data = source->getData(pos);

	bool flag = false;
	if (!source->isSolidBlockingTile(pos.west()) && data == 1) flag = true;
	if (!source->isSolidBlockingTile(pos.east()) && data == 2) flag = true;
	if (!source->isSolidBlockingTile(pos.north()) && data == 3) flag = true;
	if (!source->isSolidBlockingTile(pos.south()) && data == 4) flag = true;
	if (!source->isSolidBlockingTile(pos.below()) && data == 5) flag = true;

	if (!flag)
		return; // all good
	
	spawnResources(source, pos, source->getData(pos));
	source->setTile(pos, TILE_AIR);
}

void TorchTile::onPlace(TileSource* source, const TilePos& pos)
{
	if (source->isSolidBlockingTile(pos.west()))
		source->setTileAndData(pos, FullTile(m_ID, 1));
	else if (source->isSolidBlockingTile(pos.east()))
		source->setTileAndData(pos, FullTile(m_ID, 2));
	else if (source->isSolidBlockingTile(pos.north()))
		source->setTileAndData(pos, FullTile(m_ID, 3));
	else if (source->isSolidBlockingTile(pos.south()))
		source->setTileAndData(pos, FullTile(m_ID, 4));
	else if (source->isSolidBlockingTile(pos.below()))
		source->setTileAndData(pos, FullTile(m_ID, 5));

	checkCanSurvive(source, pos);
}

void TorchTile::setPlacedOnFace(TileSource* source, const TilePos& pos, Facing::Name face)
{
	TileData data = source->getData(pos);

	switch (face)
	{
		case Facing::UP:
			if (source->isSolidBlockingTile(pos.below()))
				data = 5;
			break;
		case Facing::NORTH:
			if (source->isSolidBlockingTile(pos.south()))
				data = 4;
			break;
		case Facing::SOUTH:
			if (source->isSolidBlockingTile(pos.north()))
				data = 3;
			break;
		case Facing::WEST:
			if (source->isSolidBlockingTile(pos.east()))
				data = 2;
			break;
		case Facing::EAST:
			if (source->isSolidBlockingTile(pos.west()))
				data = 1;
			break;
		case Facing::DOWN:
			break;
		default:
			assert(false);
			return;
	}

	source->setTileAndData(pos, FullTile(m_ID, data));
}

void TorchTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (!source->getData(pos))
		onPlace(source, pos);
}
