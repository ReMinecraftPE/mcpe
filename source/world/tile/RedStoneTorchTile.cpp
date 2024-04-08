/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RedStoneTorchTile.hpp"
#include "world/level/Level.hpp"

TorchUpdateEvents RedStoneTorchTile::m_recentUpdates;

RedStoneTorchTile::RedStoneTorchTile(int id, int texture, Material* pMtl) : TorchTile(id, texture, pMtl)
{
	m_bActive = id == TILE_NOT_GATE_ON;

	setLightEmission(0.5f);
	setTicking(true);
}

bool RedStoneTorchTile::isSignalSource()
{
	return true;
}

int RedStoneTorchTile::getTickDelay()
{
	return 2; // 2 in game ticks per redstone tick
}

void RedStoneTorchTile::onPlace(Level* level, int x, int y, int z)
{
	if (level->getData(x, y, z) == 0)
		TorchTile::onPlace(level, x, y, z);

	if (isActive())
		updateNeighbors(level, x, y, z, m_ID);
}

void RedStoneTorchTile::onRemove(Level* level, int x, int y, int z)
{
	if (isActive())
		updateNeighbors(level, x, y, z, m_ID);
}

void RedStoneTorchTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	TorchTile::neighborChanged(level, x, y, z, dir);
	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
}

int RedStoneTorchTile::getResource(int data, Random* random)
{
	return Tile::notGate->m_ID;
}

void RedStoneTorchTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (level->m_bIsMultiplayer)
		return;

	bool flag = hasSignalFromBehind(level, x, y, z);

	// remove updates that are too old
	while (!m_recentUpdates.empty())
	{
		if (level->getTime() - m_recentUpdates.front().time < 100)
			break;

		m_recentUpdates.pop_front();
	}

	if (isActive())
	{
		if (flag)
		{
			level->setTileAndData(x, y, z, Tile::notGate_off->m_ID, level->getData(x, y, z));

			if (checkBurnOut(level, x, y, z, true))
			{
				level->addParticle(
					"smoke",
					float(x) + random->nextFloat() * 0.6f + 0.2f,
					float(y) + random->nextFloat() * 0.6f + 0.2f,
					float(z) + random->nextFloat() * 0.6f + 0.2f,
					0.0f, 0.0f, 0.0f
				);
			}
		}
	}
	else if (!flag && !checkBurnOut(level, x, y, z, false))
	{
		level->setTileAndData(x, y, z, Tile::notGate->m_ID, level->getData(x, y, z));
	}
}

int RedStoneTorchTile::getDirectSignal(LevelSource* level, int x, int y, int z, int dir)
{
	if (dir == DIR_YNEG)
		return getSignal(level, x, y, z, dir);

	return 0;
}

int RedStoneTorchTile::getSignal(LevelSource* level, int x, int y, int z, int dir)
{
	if (!isActive()) return 0;
	int data = level->getData(x, y, z);

	// check for the sides _behind_ the torch
	if (data == 5 && dir == DIR_YPOS) return 0;
	if (data == 3 && dir == DIR_ZPOS) return 0;
	if (data == 4 && dir == DIR_ZNEG) return 0;
	if (data == 1 && dir == DIR_XPOS) return 0;
	if (data == 2 && dir == DIR_XNEG) return 0;
	
	return 1;
}

void RedStoneTorchTile::updateNeighbors(Level* level, int x, int y, int z, int id)
{
	level->updateNeighborsAt(x, y - 1, z, id);
	level->updateNeighborsAt(x, y + 1, z, id);
	level->updateNeighborsAt(x - 1, y, z, id);
	level->updateNeighborsAt(x + 1, y, z, id);
	level->updateNeighborsAt(x, y, z - 1, id);
	level->updateNeighborsAt(x, y, z + 1, id);
}

bool RedStoneTorchTile::hasSignalFromBehind(Level* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	if (data == 5 && level->getSignal(x, y - 1, z, DIR_YNEG)) return true;
	if (data == 3 && level->getSignal(x, y, z - 1, DIR_ZNEG)) return true;
	if (data == 4 && level->getSignal(x, y, z + 1, DIR_ZPOS)) return true;
	if (data == 1 && level->getSignal(x - 1, y, z, DIR_XNEG)) return true;
	if (data == 2 && level->getSignal(x + 1, y, z, DIR_XPOS)) return true;
	return false;
}

bool RedStoneTorchTile::checkBurnOut(Level* level, int x, int y, int z, bool b)
{
	if (b)
	{
		m_recentUpdates.push_back(TorchUpdateEvent(x, y, z, level->getTime()));
	}

	TorchUpdateEvents::iterator iter = m_recentUpdates.begin();

	int count = 0;

	for (; iter != m_recentUpdates.end(); ++iter)
	{
		// TODO: make the current level pointer also a filter
		TorchUpdateEvent& bot = *iter;
		if (bot.x == x && bot.y == y && bot.z == z && ++count == 8)
			return true;
	}

	return false;
}

void RedStoneTorchTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	float partX = float(x) + 0.5f, partZ = float(z) + 0.5f, partY = float(y) + 0.7f;

	// @NOTE: Need to use addParticle("reddust") 5 times. Invalid data values don't actually generate a smoke
	switch (level->getData(x, y, z))
	{
	case 1:
		partX -= 0.27f;
		partY += 0.22f;
		level->addParticle("reddust", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
		break;
	case 2:
		partX += 0.27f;
		partY += 0.22f;
		level->addParticle("reddust", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
		break;
	case 3:
		partZ -= 0.27f;
		partY += 0.22f;
		level->addParticle("reddust", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
		break;
	case 4:
		partZ += 0.27f;
		partY += 0.22f;
		level->addParticle("reddust", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
		break;
	case 5:
		level->addParticle("reddust", partX, partY, partZ, 0.0f, 0.0f, 0.0f);
		break;
	}
}
