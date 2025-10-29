/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RedStoneOreTile.hpp"
#include "world/level/Level.hpp"

RedStoneOreTile::RedStoneOreTile(TileID id, int texture, bool bLit) : Tile(id, texture, Material::stone)
{
	m_bLit = bLit;
	if (bLit)
		setTicking(true);
}

int RedStoneOreTile::getResource(TileData data, Random* random) const
{
	return 0;
}

int RedStoneOreTile::getResourceCount(Random* random) const
{
	return random->genrand_int32() % 2 + 4;
}

int RedStoneOreTile::getSpawnResourcesAuxValue(int x) const
{
	return 0;
}

int RedStoneOreTile::poofParticles(Level* level, const TilePos& pos)
{
	for (int i = 0; i < 6; i++)
	{
		Vec3 o(float(pos.x) + level->m_random.nextFloat(),
		       float(pos.y) + level->m_random.nextFloat(),
		       float(pos.z) + level->m_random.nextFloat());

		switch (i)
		{
			case Facing::DOWN:
				if (!level->isSolidTile(pos.above()))
					o.y = float(pos.y + 1) + 0.0625f;
				break;
			case Facing::UP:
				if (!level->isSolidTile(pos.below()))
					o.y = float(pos.y) - 0.0625f;
				break;
			case Facing::NORTH:
				if (!level->isSolidTile(pos.south()))
					o.z = float(pos.z + 1) + 0.0625f;
				break;
			case Facing::SOUTH:
				if (!level->isSolidTile(pos.north()))
					o.z = float(pos.z) - 0.0625f;
				break;
			case Facing::WEST:
				if (!level->isSolidTile(pos.east()))
					o.x = float(pos.x + 1) + 0.0625f;
				break;
			case Facing::EAST:
				if (!level->isSolidTile(pos.west()))
					o.x = float(pos.x) - 0.0625f;
				break;
		}

		if (o.x < float(pos.x) || float(pos.x + 1) < o.x || o.y < 0.0f || float(pos.y + 1) < o.y || o.z < float(pos.z) || float(pos.z + 1) < o.z)
		{
			level->addParticle("reddust", o);
		}
	}

	// @HUH: For whatever reason??
	return 1300;
}

void RedStoneOreTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	if (m_bLit)
		poofParticles(level, pos);
}

void RedStoneOreTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (m_ID == Tile::redStoneOre_lit->m_ID)
		level->setTile(pos, Tile::redStoneOre->m_ID);
}

void RedStoneOreTile::interact(Level* level, const TilePos& pos)
{
	poofParticles(level, pos);

	if (m_ID == Tile::redStoneOre->m_ID)
		level->setTile(pos, Tile::redStoneOre_lit->m_ID);
}

void RedStoneOreTile::attack(Level* level, const TilePos& pos, Player* player)
{
	interact(level, pos);
}

bool RedStoneOreTile::use(Level* level, const TilePos& pos, Player* player)
{
	interact(level, pos);
	return Tile::use(level, pos, player);
}

void RedStoneOreTile::stepOn(Level* level, const TilePos& pos, Entity* entity)
{
	interact(level, pos);
}

int RedStoneOreTile::getTickDelay() const
{
	return 30;
}
