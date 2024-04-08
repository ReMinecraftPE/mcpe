/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LeverTile.hpp"
#include "world/level/Level.hpp"

LeverTile::LeverTile(int ID, int texture, Material* pMtl) : Tile(ID, texture, pMtl)
{
}

AABB* LeverTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

int LeverTile::getRenderShape()
{
	return SHAPE_LEVER;
}

bool LeverTile::isCubeShaped()
{
	return false;
}

bool LeverTile::isSolidRender()
{
	return false;
}

bool LeverTile::checkCanSurvive(Level* level, int x, int y, int z)
{
	if (mayPlace(level, x, y, z))
		return true;

	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);

	return false;
}

HitResult LeverTile::clip(Level* level, int x, int y, int z, Vec3 a, Vec3 b)
{
	constexpr float f = 0.1875f;
	constexpr float f1 = 0.25f;

	switch (level->getData(x, y, z) & 0x7)
	{
		case 1:
			setShape(0.0f, 0.2f, 0.5f - f, f * 2.0f, 0.8f, 0.5f + f);
			break;
		case 2:
			setShape(1.0f - f * 2.0f, 0.2f, 0.5f - f, 1.0f, 0.8f, 0.5f + f);
			break;
		case 3:
			setShape(0.5f - f, 0.2f, 0.0f, 0.5f + f, 0.8f, f * 2.0f);
			break;
		case 4:
			setShape(0.5f - f, 0.2f, 1.0f - f * 2.0f, 0.5f + f, 0.8f, 1.0f);
			break;
		default:
			setShape(0.5f - f1, 0.0f, 0.5f - f1, 0.5f + f1, 0.6f, 0.5f + f1);
			break;
	}
	updateShape(level, x, y, z);
	return Tile::clip(level, x, y, z, a, b);
}

bool LeverTile::mayPlace(Level* level, int x, int y, int z)
{
	if (level->isSolidTile(x, y - 1, z)) return true;
	if (level->isSolidTile(x - 1, y, z)) return true;
	if (level->isSolidTile(x + 1, y, z)) return true;
	if (level->isSolidTile(x, y, z - 1)) return true;
	if (level->isSolidTile(x, y, z + 1)) return true;

	return false;
}

void LeverTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	if (!checkCanSurvive(level, x, y, z))
		return;

	int data = level->getData(x, y, z) & 0x7;

	bool flag = false;
	if (!level->isSolidTile(x - 1, y, z) && data == 1) flag = true;
	if (!level->isSolidTile(x + 1, y, z) && data == 2) flag = true;
	if (!level->isSolidTile(x, y, z - 1) && data == 3) flag = true;
	if (!level->isSolidTile(x, y, z + 1) && data == 4) flag = true;
	if (!level->isSolidTile(x, y - 1, z) && data == 5) flag = true;
	if (!level->isSolidTile(x, y - 1, z) && data == 6) flag = true;

	if (!flag)
		return; // all good
	
	spawnResources(level, x, y, z, level->getData(x, y, z));
	level->setTile(x, y, z, TILE_AIR);
}

void LeverTile::onPlace(Level* level, int x, int y, int z)
{
	if (level->isSolidTile(x - 1, y, z))
		level->setData(x, y, z, 1);
	else if (level->isSolidTile(x + 1, y, z))
		level->setData(x, y, z, 2);
	else if (level->isSolidTile(x, y, z - 1))
		level->setData(x, y, z, 3);
	else if (level->isSolidTile(x, y, z + 1))
		level->setData(x, y, z, 4);
	else if (level->isSolidTile(x, y - 1, z))
		level->setData(x, y, z, 5);

	checkCanSurvive(level, x, y, z);
}

void LeverTile::setPlacedOnFace(Level* level, int x, int y, int z, int dir)
{
	int data = level->getData(x, y, z);
	int bit3 = data & 0x8;
	data = -1;

	switch (dir)
	{
		case DIR_YPOS:
			if (level->isSolidTile(x, y - 1, z))
				data = 5 + level->m_random.nextInt(2);
			break;
		case DIR_ZNEG:
			if (level->isSolidTile(x, y, z + 1))
				data = 4;
			break;
		case DIR_ZPOS:
			if (level->isSolidTile(x, y, z - 1))
				data = 3;
			break;
		case DIR_XNEG:
			if (level->isSolidTile(x + 1, y, z))
				data = 2;
			break;
		case DIR_XPOS:
			if (level->isSolidTile(x - 1, y, z))
				data = 1;
			break;
	}

	if (data == -1)
	{
		spawnResources(level, x, y, z, level->getData(x, y, z));
		level->setTile(x, y, z, TILE_AIR);
		return;
	}

	level->setData(x, y, z, data | bit3);
}

int LeverTile::use(Level* pLevel, int x, int y, int z, Player* player)
{
	if (pLevel->m_bIsMultiplayer)
		return true;

	int data = pLevel->getData(x, y, z);
	data ^= 0x8;
	pLevel->setData(x, y, z, data);
	pLevel->setTilesDirty(x, y, z, x, y, z);
	pLevel->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "random.click", 0.3f, (data & 0x8) ? 0.6f : 0.5f);
	pLevel->updateNeighborsAt(x, y, z, m_ID);
	
	switch (data & 0x7)
	{
		case 1:  pLevel->updateNeighborsAt(x - 1, y, z, m_ID); break;
		case 2:  pLevel->updateNeighborsAt(x + 1, y, z, m_ID); break;
		case 3:  pLevel->updateNeighborsAt(x, y, z - 1, m_ID); break;
		case 4:  pLevel->updateNeighborsAt(x, y, z + 1, m_ID); break;
		default: pLevel->updateNeighborsAt(x, y - 1, z, m_ID); break;
	}

	return true;
}

void LeverTile::destroy(Level* pLevel, int x, int y, int z, int data)
{
	if (~data & 0x8)
	{
		Tile::destroy(pLevel, x, y, z, data);
		return;
	}

	pLevel->updateNeighborsAt(x, y, z, m_ID);

	switch (data & 0x7)
	{
		case 1:  pLevel->updateNeighborsAt(x - 1, y, z, m_ID); break;
		case 2:  pLevel->updateNeighborsAt(x + 1, y, z, m_ID); break;
		case 3:  pLevel->updateNeighborsAt(x, y, z - 1, m_ID); break;
		case 4:  pLevel->updateNeighborsAt(x, y, z + 1, m_ID); break;
		default: pLevel->updateNeighborsAt(x, y - 1, z, m_ID); break;
	}

	Tile::destroy(pLevel, x, y, z, data);
}

// TODO FIXME:
// In Minecraft JE Beta 1.6.4, WireTile::getDirectSignal is `isPoweringTo` and
// WireTile::getSignal is `isIndirectlyPoweringTo`.
//
// Meanwhile I had to reverse them, so LeverTile::getDirectSignal is actually
// `isIndirectlyPoweringTo` and vice versa...

int LeverTile::getDirectSignal(LevelSource* pLevel, int x, int y, int z, int dir)
{
	int data = pLevel->getData(x, y, z);
	if (~data & 0x8)
		return false;

	switch (data & 0x7)
	{
		case 1: return dir == DIR_XPOS;
		case 2: return dir == DIR_XNEG;
		case 3: return dir == DIR_ZPOS;
		case 4: return dir == DIR_ZNEG;
		case 5:
		case 6: return dir == DIR_YPOS;
	}

	return false;
}

int LeverTile::getSignal(LevelSource* pLevel, int x, int y, int z, int dir)
{
	int data = pLevel->getData(x, y, z);
	return (data & 0x8) != 0;
}

bool LeverTile::isSignalSource()
{
	return true;
}
