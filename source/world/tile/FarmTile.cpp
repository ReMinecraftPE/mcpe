/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FarmTile.hpp"
#include "world/level/Level.hpp"

FarmTile::FarmTile(TileID id, Material* c) : Tile(id, c)
{
	m_TextureFrame = TEXTURE_FARMLAND_DRY;

	setTicking(true);
	setShape(0, 0, 0, 1, 15.0f / 16.0f, 1.0f);
	setLightBlock(255);
}

AABB* FarmTile::getAABB(const Level*, const TilePos& pos)
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

bool FarmTile::isNearWater(Level* level, const TilePos& pos)
{
	TilePos waterPos = TilePos();

	for (waterPos.x = pos.x - 4; waterPos.x <= pos.x + 4; waterPos.x++)
	{
		for (waterPos.y = pos.y; waterPos.y <= pos.y + 1; waterPos.y++)
		{
			for (waterPos.z = pos.z - 4; waterPos.z <= pos.z + 4; waterPos.z++)
			{
				if (level->getMaterial(pos) == Material::water)
					return true;
			}
		}
	}

	return false;
}

void FarmTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (level->getMaterial(pos.above())->isSolid())
		level->setTile(pos, Tile::dirt->m_ID);
}

void FarmTile::stepOn(Level* level, const TilePos& pos, Entity* pEnt)
{
	if (level->m_random.genrand_int32() % 4 == 0)
		level->setTile(pos, Tile::dirt->m_ID);
}

void FarmTile::tick(Level* level, const TilePos& pos, Random* random)
{
	int val = random->genrand_int32();

	//@HUH: weird way of saying val % 5 == 0
	if (val != 5 * (val / 5))
		return;

	if (isNearWater(level, pos))
	{
		level->setData(pos, 7);
	}
	else
	{
		TileData data = level->getData(pos);

		if (data <= 0)
			level->setTile(pos, Tile::dirt->m_ID);
		else
			level->setData(pos, data - 1);
	}
}
