/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PressurePlateTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Entity.hpp"

PressurePlateTile::PressurePlateTile(int id, int texture, eCheckedEntities ch, Material* pMtl) :
	Tile(id, texture, pMtl),
	m_checkedEntities(ch)
{
	setTicking(true);
	setShape(0.0625f, 0.0f, 0.0625f, 0.9375f, 0.03125f, 0.9375f);
}

AABB* PressurePlateTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

int PressurePlateTile::getTickDelay()
{
	return 20;
}

bool PressurePlateTile::isSolidRender()
{
	return false;
}

bool PressurePlateTile::isCubeShaped()
{
	return false;
}

bool PressurePlateTile::mayPlace(Level* level, int x, int y, int z)
{
	return level->isSolidTile(x, y - 1, z);
}

void PressurePlateTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (level->isSolidTile(x, y - 1, z))
		return;
	
	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
}

void PressurePlateTile::updateShape(LevelSource* level, int x, int y, int z)
{
	if (level->getData(x, y, z) == 1)
		setShape(0.0625f, 0.0f, 0.0625f, 0.9375f, 0.03125f, 0.9375f);
	else
		setShape(0.0625f, 0.0f, 0.0625f, 0.9375f, 0.0625f, 0.9375f);
}

void PressurePlateTile::destroy(Level* level, int x, int y, int z, int data)
{
	if (data > 0)
	{
		level->updateNeighborsAt(x, y, z, m_ID);
		level->updateNeighborsAt(x, y - 1, z, m_ID);
	}

	Tile::destroy(level, x, y, z, data);
}

int PressurePlateTile::getSignal(LevelSource* level, int x, int y, int z, int dir)
{
	return level->getData(x, y, z) > 0;
}

int PressurePlateTile::getDirectSignal(LevelSource* level, int x, int y, int z, int dir)
{
	return dir == DIR_YPOS && level->getData(x, y, z) > 0;
}

bool PressurePlateTile::isSignalSource()
{
	return true;
}

void PressurePlateTile::tick(Level* level, int x, int y, int z, Random* random)
{
	// Multiplayer will release the plate for us.
	if (level->m_bIsMultiplayer)
		return;

	// If plate is already released
	if (level->getData(x, y, z) == 0)
		return;

	checkStep(level, x, y, z);
}

void PressurePlateTile::updateDefaultShape()
{
	constexpr float f0 = 0.5f, f1 = 0.125f, f2 = 0.5f;
	setShape(0.5f - f0, 0.5f - f1, 0.5f - f2, 0.5f + f0, 0.5f + f1, 0.5f + f2);
}

void PressurePlateTile::entityInside(Level* level, int x, int y, int z, Entity* entity)
{
	if (level->m_bIsMultiplayer)
		return;

	if (level->getData(x, y, z) == 1)
		return;

	checkStep(level, x, y, z);
}

void PressurePlateTile::checkStep(Level* level, int x, int y, int z)
{
	bool wasOn = level->getData(x, y, z) == 1;
	constexpr float offs = 0.125f;

	AABB aabb(float(x) + offs, float(y), float(z) + offs, float(x + 1) - offs, float(y) + 0.25f, float(z + 1) - offs);
	EntityVector* entities = level->getEntities(nullptr, aabb);

	bool isOn = false;
	switch (m_checkedEntities)
	{
		case CHECK_EVERYTHING:
			isOn = !entities->empty();
			break;

		case CHECK_MOBS:
			isOn = false;
			for (size_t i = 0; i < entities->size(); i++)
			{
				if ((*entities)[i]->isMob())
				{
					isOn = true;
					break;
				}
			}
			break;

		case CHECK_PLAYERS:
			isOn = false;
			for (size_t i = 0; i < entities->size(); i++)
			{
				if ((*entities)[i]->isPlayer())
				{
					isOn = true;
					break;
				}
			}
			break;
	}

	if (isOn && !wasOn)
	{
		level->setData(x, y, z, 1);
		level->updateNeighborsAt(x, y, z, m_ID);
		level->updateNeighborsAt(x, y - 1, z, m_ID);
		level->setTilesDirty(x, y, z, x, y, z);
		level->playSound(float(x) + 0.5f, float(y) + 0.1f, float(z) + 0.5f, "random.click", 0.3f, 0.6f);
	}
	if (!isOn && wasOn)
	{
		level->setData(x, y, z, 0);
		level->updateNeighborsAt(x, y, z, m_ID);
		level->updateNeighborsAt(x, y - 1, z, m_ID);
		level->setTilesDirty(x, y, z, x, y, z);
		level->playSound(float(x) + 0.5f, float(y) + 0.1f, float(z) + 0.5f, "random.click", 0.3f, 0.5f);
	}

	if (isOn)
	{
		level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
	}
}
