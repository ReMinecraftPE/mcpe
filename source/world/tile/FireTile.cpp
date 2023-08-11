/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FireTile.hpp"
#include "world/level/Level.hpp"

FireTile::FireTile(int ID, int texture) : Tile(ID, texture, Material::fire)
{
	// @BUG: Uninitialized memory can cause the odds of normally inflammable items to go
	// out of reasonable limits, therefore causing the bug demonstrated in the following video:
	// https://www.youtube.com/watch?v=3hrz7KK2EJs
#ifndef ORIGINAL_CODE
	memset(m_igniteOdds, 0, sizeof m_igniteOdds);
	memset(m_burnOdds, 0, sizeof m_burnOdds);
#endif

	m_igniteOdds[Tile::wood->m_ID]      = 5;
	m_burnOdds  [Tile::wood->m_ID]      = 20;
	m_igniteOdds[Tile::treeTrunk->m_ID] = 5;
	m_burnOdds  [Tile::treeTrunk->m_ID] = 5;
	m_igniteOdds[Tile::leaves->m_ID]    = 30;
	m_burnOdds  [Tile::leaves->m_ID]    = 60;
	m_igniteOdds[Tile::tnt->m_ID]       = 15;
	m_burnOdds  [Tile::tnt->m_ID]       = 100;
	m_igniteOdds[Tile::cloth->m_ID]     = 30;
	m_burnOdds  [Tile::cloth->m_ID]     = 60;
	// @NOTE: Not setting the other cloths' properties

	setTicking(true);
}

int FireTile::getRenderShape()
{
	// @BUG: Since the shape is set to FIRE, but TileRenderer doesn't handle it,
	// fire is invisible in this version of Minecraft.
	return SHAPE_FIRE;
}

bool FireTile::isSolidRender()
{
	return false;
}

bool FireTile::isCubeShaped()
{
	return false;
}

AABB* FireTile::getAABB(Level* level, int x, int y, int z)
{
	return nullptr;
}

int FireTile::getResourceCount(Random* random)
{
	return 0;
}

int FireTile::getTickDelay()
{
	return 10;
}

void FireTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	// @TODO: Mark Tile::fire as FireTile* instead of Tile*
	FireTile* pFireTile = (FireTile*)Tile::fire;
	if (level->isSolidTile(x, y - 1, z) || pFireTile->canBurn(level, x, y - 1, z))
	{
		for (int i = 0; i < 3; i++)
		{
			level->addParticle("largesmoke", float(x) + random->nextFloat(), float(y) + random->nextFloat() * 0.5f + 0.5f, float(z) + random->nextFloat(), 0.0f, 0.0f, 0.0f);
		}

		return;
	}

	if (pFireTile->canBurn(level, x - 1, y, z))
		level->addParticle("largesmoke", float(x) + random->nextFloat() * 0.1f, float(y) + random->nextFloat(), float(z) + random->nextFloat(), 0.0f, 0.0f, 0.0f);
	if (pFireTile->canBurn(level, x + 1, y, z))
		level->addParticle("largesmoke", float(x + 1) - random->nextFloat() * 0.1f, float(y) + random->nextFloat(), float(z) + random->nextFloat(), 0.0f, 0.0f, 0.0f);
	if (pFireTile->canBurn(level, x, y, z - 1))
		level->addParticle("largesmoke", float(x) + random->nextFloat(), float(y) + random->nextFloat(), float(z) + random->nextFloat() * 0.1f, 0.0f, 0.0f, 0.0f);
	if (pFireTile->canBurn(level, x, y, z + 1))
		level->addParticle("largesmoke", float(x) + random->nextFloat(), float(y) + random->nextFloat(), float(z + 1) - random->nextFloat() * 0.1f, 0.0f, 0.0f, 0.0f);
	if (pFireTile->canBurn(level, x, y + 1, z))
		level->addParticle("largesmoke", float(x) + random->nextFloat(), float(y + 1) - random->nextFloat() * 0.1f, float(z) + random->nextFloat(), 0.0f, 0.0f, 0.0f);
}

void FireTile::checkBurn(Level* level, int x, int y, int z, int thing, Random* random)
{
	if (m_burnOdds[level->getTile(x, y, z)] > int(random->nextInt(thing)))
	{
		TileID tid = level->getTile(x, y, z);

		TileID newTile = m_ID;
		if (random->nextInt(2))
			newTile = TILE_AIR;

		level->setTile(x, y, z, newTile);
		if (tid == Tile::tnt->m_ID)
			Tile::tnt->destroy(level, x, y, z, 0);
	}
}

int FireTile::getFireOdds(Level* level, int x, int y, int z)
{
	if (!level->isEmptyTile(x, y, z))
		return 0;

	int odds = m_igniteOdds[level->getTile(x + 1, y, z)], o;
	if (odds < 0)
		odds = 0;

	o = m_igniteOdds[level->getTile(x - 1, y, z)];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(x, y - 1, z)];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(x, y + 1, z)];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(x, y, z - 1)];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(x, y, z + 1)];
	if (odds < o) odds = o;

	return odds;
}

bool FireTile::isValidFireLocation(Level* level, int x, int y, int z)
{
	if (canBurn(level, x + 1, y, z)) return true;
	if (canBurn(level, x - 1, y, z)) return true;
	if (canBurn(level, x, y - 1, z)) return true;
	if (canBurn(level, x, y + 1, z)) return true;
	if (canBurn(level, x, y, z - 1)) return true;
	if (canBurn(level, x, y, z + 1)) return true;
	return false;
}

bool FireTile::mayPick()
{
	return false;
}

bool FireTile::mayPlace(Level* level, int x, int y, int z)
{
	// @NOTE: This is useless as you usually don't 'place' fire.
	if (level->isSolidTile(x, y - 1, z))
		return true;

	return isValidFireLocation(level, x, y, z);
}

void FireTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (!level->isSolidTile(x, y - 1, z) && !isValidFireLocation(level, x, y, z))
		level->setTile(x, y, z, TILE_AIR);
}

void FireTile::onPlace(Level* level, int x, int y, int z)
{
	// @NOTE: Unused return result
	level->getTile(x, y - 1, z);

	if (!level->isSolidTile(x, y - 1, z) && !isValidFireLocation(level, x, y, z))
	{
		level->setTile(x, y, z, TILE_AIR);
		return;
	}

	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
}

void FireTile::tick(Level* level, int x, int y, int z, Random* random)
{
	int data = level->getData(x, y, z);
	if (data <= 14)
	{
		level->setData(x, y, z, data + 1);
		level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
	}

	if (isValidFireLocation(level, x, y, z))
	{
		if (m_igniteOdds[level->getTile(x, y - 1, z)] <= 0 && data == 15 && !random->nextInt(4))
		{
			// just go out
			level->setTile(x, y, z, TILE_AIR);
			return;
		}

		if (data > 2 && (data & 1) == 0)
		{
			checkBurn(level, x + 1, y, z, 300, random);
			checkBurn(level, x - 1, y, z, 300, random);
			checkBurn(level, x, y - 1, z, 250, random);
			checkBurn(level, x, y + 1, z, 250, random);
			checkBurn(level, x, y, z - 1, 300, random);
			checkBurn(level, x, y, z + 1, 300, random);

			for (int xi = x - 1; xi <= x + 1; xi++)
			{
				for (int zi = z - 1; zi <= z + 1; zi++)
				{
					for (int yi = y - 1, thing = -100; yi <= y + 4; yi++, thing += 100)
					{
						if (yi == y && xi == x && zi == z)
							continue;

						int thing2 = y + 1 >= yi ? 100 : thing;
						int odds = getFireOdds(level, xi, yi, zi);
						if (odds > 0 && odds >= int(random->nextInt(thing2)))
							level->setTile(xi, yi, zi, m_ID);
					}
				}
			}
		}
		if (data == 15)
		{
			checkBurn(level, x + 1, y, z, 1, random);
			checkBurn(level, x - 1, y, z, 1, random);
			checkBurn(level, x, y - 1, z, 1, random);
			checkBurn(level, x, y + 1, z, 1, random);
			checkBurn(level, x, y, z - 1, 1, random);
			checkBurn(level, x, y, z + 1, 1, random);
		}
	}
	else if (!level->isSolidTile(x, y - 1, z) || data > 3)
	{
		level->setTile(x, y, z, TILE_AIR);
	}
}
