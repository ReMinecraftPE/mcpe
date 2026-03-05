/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FarmTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"

FarmTile::FarmTile(TileID id, Material* c) : Tile(id, c)
{
	m_TextureFrame = TEXTURE_FARMLAND_DRY;

	setTicking(true);
	setShape(0, 0, 0, 1, 15.0f / 16.0f, 1.0f);
	setLightBlock(255);
}

AABB* FarmTile::getAABB(TileSource*, const TilePos& pos)
{
	// a full block
	m_aabbReturned = AABB(
		float(pos.x),
		float(pos.y),
		float(pos.z),
		float(pos.x + 1),
		float(pos.y + 1),
		float(pos.z + 1)
	);

	return &m_aabbReturned;
}

int FarmTile::getResource(TileData data, Random* random) const
{
	return Tile::dirt->getResource(data, random);
}

int FarmTile::getTexture(Facing::Name face, TileData data) const
{
	if (face == Facing::UP)
	{
		if (data > 0)
			return m_TextureFrame - 1;

		return m_TextureFrame;
	}

	return TEXTURE_DIRT;
}

bool FarmTile::isCubeShaped() const
{
	return false;
}

bool FarmTile::isSolidRender() const
{
	return false;
}

bool FarmTile::isNearWater(TileSource* source, const TilePos& pos)
{
	TilePos waterPos = TilePos();

	for (waterPos.x = pos.x - 4; waterPos.x <= pos.x + 4; waterPos.x++)
	{
		for (waterPos.y = pos.y; waterPos.y <= pos.y + 1; waterPos.y++)
		{
			for (waterPos.z = pos.z - 4; waterPos.z <= pos.z + 4; waterPos.z++)
			{
				if (source->getMaterial(waterPos) == Material::water)
					return true;
			}
		}
	}

	return false;
}

bool FarmTile::isUnderCrops(TileSource* source, const TilePos& pos)
{
	return source->getTile(pos.above()) == Tile::crops->m_ID;
}

void FarmTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	if (source->getMaterial(pos.above())->isSolid())
		source->setTile(pos, Tile::dirt->m_ID);
}

void FarmTile::stepOn(TileSource* source, const TilePos& pos, Entity* pEnt)
{
	if (source->getLevel().m_random.genrand_int32() % 4 == 0)
		source->setTile(pos, Tile::dirt->m_ID);
}

void FarmTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getLevelConst().m_bIsClientSide)
		return;

	if (random->nextInt(5) != 0)
		return;

	if (isNearWater(source, pos)/* && !level->isRainingAt(pos.above())*/)
	{
		source->setTileAndData(pos, FullTile(m_ID, 7));
		return;
	}

	TileData data = source->getData(pos);

	if (data > 0)
		source->setTileAndData(pos, FullTile(m_ID, data - 1));
	else if (!isUnderCrops(source, pos))
		source->setTile(pos, Tile::dirt->m_ID);
}
