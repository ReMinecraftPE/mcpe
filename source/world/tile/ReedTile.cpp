/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ReedTile.hpp"
#include "world/level/Level.hpp"

ReedTile::ReedTile(int id) : Tile(id, Material::plant)
{
	m_TextureFrame = TEXTURE_REEDS;
	setShape(0.125f, 0.0f, 0.125f, 0.875f, 1.0f, 0.875f);
	setTicking(true);
}

int ReedTile::getRenderShape() const
{
	return 1;
}

bool ReedTile::isCubeShaped() const
{
	return false;
}

bool ReedTile::isSolidRender() const
{
	return false;
}

bool ReedTile::mayPlace(const Level* level, const TilePos& pos) const
{
	TileID tileBelow = level->getTile(pos.below());

	if (tileBelow == m_ID)
		return true;

	//@NOTE: No sand
	if (tileBelow != Tile::grass->m_ID && tileBelow != Tile::dirt->m_ID)
		return false;

	return
		level->getMaterial(pos.below().west()) == Material::water ||
		level->getMaterial(pos.below().east()) == Material::water ||
		level->getMaterial(pos.below().north()) == Material::water ||
		level->getMaterial(pos.below().south()) == Material::water;
}

bool ReedTile::canSurvive(const Level* level, const TilePos& pos) const
{
	return mayPlace(level, pos);
}

void ReedTile::checkAlive(Level* level, const TilePos& pos)
{
	if (!canSurvive(level, pos))
	{
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}
}

void ReedTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	return checkAlive(level, pos);
}

void ReedTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (!level->isEmptyTile(pos))
		return;

	int height;
	for (height = 1; level->getTile(TilePos(pos.x, pos.y - height, pos.z)) == m_ID; height++);

	if (height <= 2)
	{
		int data = level->getData(pos);

		if (data == 15)
		{
			level->setTile(pos.above(), m_ID);
			level->setData(pos, 0);
		}
		else
		{
			level->setData(pos, data + 1);
		}
	}
}

AABB* ReedTile::getAABB(const Level* level, const TilePos& pos)
{
	return nullptr;
}

int ReedTile::getResource(int x, Random* random) const
{
	return Item::reeds->m_itemID;
}
