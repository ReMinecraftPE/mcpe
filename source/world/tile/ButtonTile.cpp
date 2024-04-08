/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ButtonTile.hpp"
#include "world/level/Level.hpp"

ButtonTile::ButtonTile(int id, int texture) : Tile(id, texture, Material::decoration)
{
	setTicking(true);
}

AABB* ButtonTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

int ButtonTile::getTickDelay()
{
	return 20;
}

bool ButtonTile::isSolidRender()
{
	return false;
}

bool ButtonTile::isCubeShaped()
{
	return false;
}

bool ButtonTile::mayPlace(Level* level, int x, int y, int z)
{
	if (level->isSolidTile(x - 1, y, z)) return true;
	if (level->isSolidTile(x + 1, y, z)) return true;
	if (level->isSolidTile(x, y, z - 1)) return true;
	if (level->isSolidTile(x, y, z + 1)) return true;
	return false;
}

void ButtonTile::onPlace(Level* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	int onBit = data & 8;

	if (level->isSolidTile(x - 1, y, z))
		level->setData(x, y, z, onBit | 1);
	else if (level->isSolidTile(x + 1, y, z))
		level->setData(x, y, z, onBit | 2);
	else if (level->isSolidTile(x, y, z - 1))
		level->setData(x, y, z, onBit | 3);
	else if (level->isSolidTile(x, y, z + 1))
		level->setData(x, y, z, onBit | 4);

	checkCanSurvive(level, x, y, z);
}

void ButtonTile::setPlacedOnFace(Level* level, int x, int y, int z, int dir)
{
	int data = level->getData(x, y, z);
	int onBit = data & 8;
	data &= 7;

	/**/ if (dir == 2 && level->isSolidTile(x, y, z + 1)) data = 4;
	else if (dir == 3 && level->isSolidTile(x, y, z - 1)) data = 3;
	else if (dir == 4 && level->isSolidTile(x + 1, y, z)) data = 2;
	else if (dir == 5 && level->isSolidTile(x - 1, y, z)) data = 1;
	else data = 1;

	level->setData(x, y, z, data | onBit);

	checkCanSurvive(level, x, y, z);
}

void ButtonTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (!checkCanSurvive(level, x, y, z))
		return;

	int data = level->getData(x, y, z) & 7;
	bool kill = false;
	if (!level->isSolidTile(x - 1, y, z) && data == 1) kill = true;
	if (!level->isSolidTile(x + 1, y, z) && data == 2) kill = true;
	if (!level->isSolidTile(x, y, z - 1) && data == 3) kill = true;
	if (!level->isSolidTile(x, y, z + 1) && data == 4) kill = true;

	if (!kill)
		return;
	
	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
}

bool ButtonTile::checkCanSurvive(Level* level, int x, int y, int z)
{
	if (mayPlace(level, x, y, z))
		return true;

	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
	return false;
}

void ButtonTile::updateShape(LevelSource* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	int dir = data & 0x7;
	bool on = data & 0x8;

	constexpr float f = 0.375f;
	constexpr float f1 = 0.625f;
	constexpr float f2 = 0.1875f;
	float offs = (on ? 1.0f : 2.0f) / 16.0f;

	switch (dir)
	{
		case 1:
			setShape(0.0f, f, 0.5f - f2, offs, f1, 0.5f + f2);
			break;
		case 2:
			setShape(1.0f - offs, f, 0.5f - f2, 1.0f, f1, 0.5f + f2);
			break;
		case 3:
			setShape(0.5f - f2, f, 0.0f, 0.5f + f2, f1, offs);
			break;
		case 4:
			setShape(0.5f - f2, f, 1.0f - offs, 0.5f + f2, f1, 1.0f);
			break;
	}
}

int ButtonTile::use(Level* level, int x, int y, int z, Player* player)
{
	int data = level->getData(x, y, z);
	int dir = data & 7;
	int onBit = data & 0x8;
	onBit ^= 0x8;

	if (onBit == 0)
		return 1;
	
	level->setData(x, y, z, dir | onBit);
	level->setTilesDirty(x, y, z, x, y, z);
	level->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "random.click", 0.3f, 0.6f);
	level->updateNeighborsAt(x, y, z, m_ID);

	switch (dir)
	{
		case 1:
			level->updateNeighborsAt(x - 1, y, z, m_ID);
			break;
		case 2:
			level->updateNeighborsAt(x + 1, y, z, m_ID);
			break;
		case 3:
			level->updateNeighborsAt(x, y, z - 1, m_ID);
			break;
		case 4:
			level->updateNeighborsAt(x, y, z + 1, m_ID);
			break;
		default:
			level->updateNeighborsAt(x, y - 1, z, m_ID);
			break;
	}

	level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
	return 1;
}

void ButtonTile::destroy(Level* level, int x, int y, int z, int pdir)
{
	if ((pdir & 8) > 0)
	{
		level->updateNeighborsAt(x, y, z, m_ID);
		switch (pdir & 0x7)
		{
			case 1:
				level->updateNeighborsAt(x - 1, y, z, m_ID);
				break;
			case 2:
				level->updateNeighborsAt(x + 1, y, z, m_ID);
				break;
			case 3:
				level->updateNeighborsAt(x, y, z - 1, m_ID);
				break;
			case 4:
				level->updateNeighborsAt(x, y, z + 1, m_ID);
				break;
			default:
				level->updateNeighborsAt(x, y - 1, z, m_ID);
				break;
		}
	}

	Tile::destroy(level, x, y, z, pdir);
}

int ButtonTile::getSignal(LevelSource* level, int x, int y, int z, int dir)
{
	return (level->getData(x, y, z) & 0x8) > 0;
}

int ButtonTile::getDirectSignal(LevelSource* level, int x, int y, int z, int dir)
{
	int data = level->getData(x, y, z);
	if (~data & 0x8)
		return false;
	
	int bdir = data & 0x7;
	if (bdir == 5 && dir == DIR_YPOS) return true;
	if (bdir == 4 && dir == DIR_ZNEG) return true;
	if (bdir == 3 && dir == DIR_ZPOS) return true;
	if (bdir == 2 && dir == DIR_XNEG) return true;
	if (bdir == 1 && dir == DIR_XPOS) return true;

	return false;
}

bool ButtonTile::isSignalSource()
{
	return true;
}

void ButtonTile::tick(Level* level, int x, int y, int z, Random* random)
{
	// Multiplayer will release the button for us.
	if (level->m_bIsMultiplayer)
		return;
	
	int data = level->getData(x, y, z);

	// If button is already released:
	if (~data & 0x8)
		return;
	
	// Release the button now.
	level->setData(x, y, z, data & 0x7);
	level->updateNeighborsAt(x, y, z, m_ID);

	switch (data & 0x7)
	{
		case 1:
			level->updateNeighborsAt(x - 1, y, z, m_ID);
			break;
		case 2:
			level->updateNeighborsAt(x + 1, y, z, m_ID);
			break;
		case 3:
			level->updateNeighborsAt(x, y, z - 1, m_ID);
			break;
		case 4:
			level->updateNeighborsAt(x, y, z + 1, m_ID);
			break;
		default:
			level->updateNeighborsAt(x, y - 1, z, m_ID);
			break;
	}

	level->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "random.click", 0.3f, 0.5f);
	level->setTilesDirty(x, y, z, x, y, z);
}

void ButtonTile::updateDefaultShape()
{
	constexpr float f0 = 0.1875f, f1 = 0.125f, f2 = 0.125f;
	setShape(0.5f - f0, 0.5f - f1, 0.5f - f2, 0.5f + f0, 0.5f + f1, 0.5f + f2);
}
