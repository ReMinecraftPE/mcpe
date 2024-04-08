/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DiodeTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Entity.hpp"

// Fun fact: At one point this used to be 1,2,5,7.
// These numbers are in redstone ticks. A redstone tick is two game ticks.
int DiodeTile::m_delays[] = { 1, 2, 3, 4 };

float DiodeTile::m_particleOffsets[] = { -0.0625f, 0.0625f, 0.1875f, 0.3125f };

DiodeTile::DiodeTile(int id, bool on) :
	Tile(id, 6, Material::decoration)
{
	m_bActive = on;
	setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.125f, 1.0f);
}

bool DiodeTile::isCubeShaped()
{
	return false;
}

bool DiodeTile::mayPlace(Level* level, int x, int y, int z)
{
	if (!level->isSolidTile(x, y - 1, z))
		return false;

	return Tile::mayPlace(level, x, y, z);
}

bool DiodeTile::canSurvive(Level* level, int x, int y, int z)
{
	if (!level->isSolidTile(x, y - 1, z))
		return false;

	return Tile::canSurvive(level, x, y, z);
}

bool DiodeTile::hasSignal(Level* level, int x, int y, int z, int data)
{
	switch (data & 3)
	{
		case 0:
			return level->getSignal(x, y, z + 1, DIR_ZPOS);
		case 1:
			return level->getSignal(x - 1, y, z, DIR_XNEG);
		case 2:
			return level->getSignal(x, y, z - 1, DIR_ZNEG);
		case 3:
			return level->getSignal(x + 1, y, z, DIR_XPOS);
	}

	return false;
}

void DiodeTile::tick(Level* level, int x, int y, int z, Random* random)
{
	int data = level->getData(x, y, z);
	bool pow = hasSignal(level, x, y, z, data);

	if (m_bActive && !pow)
	{
		level->setTileAndData(x, y, z, Tile::repeater_off->m_ID, data);
	}
	else if (!m_bActive)
	{
		level->setTileAndData(x, y, z, Tile::repeater_on->m_ID, data);
		if (!pow)
		{
			int delayIndex = (data & 0b1100) >> 2;
			level->addToTickNextTick(x, y, z, Tile::repeater_on->m_ID, m_delays[delayIndex] * 2);
		}
	}
}

int DiodeTile::getTexture(int dir, int data)
{
	if (dir == DIR_YNEG)
		return m_bActive ? TEXTURE_TORCH_RED_STONE : TEXTURE_TORCH_RED_STONE_OFF;

	if (dir == DIR_YPOS)
		return m_bActive ? TEXTURE_DIODE_ON : TEXTURE_DIODE_OFF;

	return TEXTURE_STONE_SLAB_SIDE;
}

bool DiodeTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int dir)
{
	return dir != DIR_YNEG && dir != DIR_YPOS;
}

int DiodeTile::getRenderShape()
{
	return SHAPE_DIODE;
}

int DiodeTile::getTexture(int dir)
{
	return getTexture(dir, 0);
}

int DiodeTile::getSignal(LevelSource* level, int x, int y, int z, int dir)
{
	return getDirectSignal(level, x, y, z, dir);
}

int DiodeTile::getDirectSignal(LevelSource* level, int x, int y, int z, int dir)
{
	if (!m_bActive)
		return false;

	int rdir = level->getData(x, y, z) & 0b0011;
	if (dir == DIR_ZPOS && rdir == 0) return true;
	if (dir == DIR_XNEG && rdir == 1) return true;
	if (dir == DIR_ZNEG && rdir == 2) return true;
	if (dir == DIR_XPOS && rdir == 3) return true;
	return false;
}

void DiodeTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (!canSurvive(level, x, y, z))
	{
		spawnResources(level, x, y, z, level->getData(x, y, z));
		level->setTile(x, y, z, TILE_AIR);
		return;
	}

	int data = level->getData(x, y, z);
	bool pow = hasSignal(level, x, y, z, data);
	int delay = (data & 0b1100) >> 2;

	if (m_bActive != pow)
		level->addToTickNextTick(x, y, z, m_ID, 2 * m_delays[delay]);
}

int DiodeTile::use(Level* level, int x, int y, int z, Player* player)
{
	int data = level->getData(x, y, z);
	data = (data & 0b0011) | ((data + 0b0100) & 0b1100);
	level->setData(x, y, z, data);
	return true;
}

bool DiodeTile::isSignalSource()
{
	return false; // wth??
}

void DiodeTile::setPlacedBy(Level* level, int x, int y, int z, Mob* mob)
{
	int dir = (Mth::floor(mob->m_yaw * 4.0f / 360.0f + 0.5f) + 2) & 3;

	level->setData(x, y, z, dir);

	if (hasSignal(level, x, y, z, dir))
		level->addToTickNextTick(x, y, z, m_ID, 1);
}

void DiodeTile::onPlace(Level* level, int x, int y, int z)
{
	level->updateNeighborsAt(x + 1, y, z, m_ID);
	level->updateNeighborsAt(x - 1, y, z, m_ID);
	level->updateNeighborsAt(x, y, z + 1, m_ID);
	level->updateNeighborsAt(x, y, z - 1, m_ID);
	level->updateNeighborsAt(x, y + 1, z, m_ID);
	level->updateNeighborsAt(x, y - 1, z, m_ID);
}

bool DiodeTile::isSolidRender()
{
	return false;
}

int DiodeTile::getResource(int data, Random* random)
{
	return Item::diode->m_itemID;
}

void DiodeTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	if (!m_bActive)
		return;

	int data = level->getData(x, y, z);
	float px = float(x) + 0.5f + 0.2f * (random->nextFloat() - 0.5f);
	float py = float(y) + 0.5f + 0.2f * (random->nextFloat() - 0.5f);
	float pz = float(z) + 0.5f + 0.2f * (random->nextFloat() - 0.5f);
	float xo = 0.0f;
	float zo = 0.0f;

	if (random->nextInt(2) == 0)
	{
		switch (data & 0b0011)
		{
			case 0: zo = -0.3125f; break;
			case 2: zo = +0.3125f; break;
			case 3: xo = -0.3125f; break;
			case 1: xo = +0.3125f; break;
		}
	}
	else
	{
		int delay = (data & 0b1100) >> 2;
		switch (data & 0b0011)
		{
			case 0: zo = -m_particleOffsets[delay]; break;
			case 2: zo = +m_particleOffsets[delay]; break;
			case 3: xo = -m_particleOffsets[delay]; break;
			case 1: xo = +m_particleOffsets[delay]; break;
		}
	}

	level->addParticle("reddust", px + xo, py, pz + zo, 0.0f, 0.0f, 0.0f);
}
