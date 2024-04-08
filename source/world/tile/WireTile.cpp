/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "WireTile.hpp"
#include "world/level/Level.hpp"

#ifdef __APPLE__

// HACK HACK HACK: This is crap

// Include other C++ sources directly, because Apple made it purposely
// difficult to add more source files by modifying the PBXPROJ.  I
// have to allocate GUIDs and .. ugh, it's a pain.  Apple can suck
// a big, fat one.
//
// Brent, please help.  I don't want to merge it into master like this.
#include "RedStoneTorchTile.cpp"
#include "LeverTile.cpp"
#include "ButtonTile.cpp"
#include "PressurePlateTile.cpp"
#include "DiodeTile.cpp"

#endif

WireTile::WireTile(int id) : Tile(id, Material::decoration)
{
	m_TextureFrame = 164;
	m_bIsPowerSource = true;
}

bool WireTile::isSolidRender()
{
	return false;
}

bool WireTile::isCubeShaped()
{
	return false;
}

bool WireTile::isSignalSource()
{
	return m_bIsPowerSource;
}

bool WireTile::canSurvive(Level* level, int x, int y, int z)
{
	return level->isSolidTile(x, y - 1, z);
}

bool WireTile::isSignalSource(LevelSource* level, int x, int y, int z)
{
	int tile = level->getTile(x, y, z);
	if (tile == m_ID) return true;
	if (!tile) return false;
	return Tile::tiles[tile]->isSignalSource();
}

int WireTile::getConnections(LevelSource* level, int x, int y, int z)
{
	// Determine connection between wires.
	int connFlags = 0;
	const int checkXD[] = { -1, +1, 0, 0 };
	const int checkZD[] = { 0, 0, -1, +1 };

	bool bIsSolidTileAbove = level->isSolidTile(x, y + 1, z);

	for (int i = 0; i < CONN_COUNT; i++)
	{
		Tile* tile;

		tile = Tile::tiles[level->getTile(x + checkXD[i], y, z + checkZD[i])];
		if (tile && tile->isSignalSource())
		{
			connFlags |= (1 << i);
			continue;
		}

		// check above
		if (!bIsSolidTileAbove)
		{
			if (level->getTile(x + checkXD[i], y + 1, z + checkZD[i]) == m_ID)
			{
				connFlags |= (1 << i) | (1 << (i + 4));
				continue;
			}
		}

		// check below:
		if (level->isSolidTile(x + checkXD[i], y, z + checkZD[i]))
			continue;

		if (level->getTile(x + checkXD[i], y - 1, z + checkZD[i]) == m_ID)
		{
			connFlags |= (1 << i);
			continue;
		}
	}

	// If we have only one flag, set the opposite too.
	if ((connFlags & CONN_MASK) == (1 << CONN_XN)) connFlags |= (1 << CONN_XP);
	if ((connFlags & CONN_MASK) == (1 << CONN_XP)) connFlags |= (1 << CONN_XN);
	if ((connFlags & CONN_MASK) == (1 << CONN_ZN)) connFlags |= (1 << CONN_ZP);
	if ((connFlags & CONN_MASK) == (1 << CONN_ZP)) connFlags |= (1 << CONN_ZN);

	return connFlags;
}

void WireTile::recalculate(Level* level, int x, int y, int z)
{
	calculateChanges(level, x, y, z, x, y, z);

	std::vector<TilePos> tpv(m_positionsToUpdate.begin(), m_positionsToUpdate.end());
	m_positionsToUpdate.clear();

	std::vector<TilePos>::iterator it;
	for (it = tpv.begin();
		it != tpv.end();
		++it)
	{
		level->updateNeighborsAt(it->x, it->y, it->z, m_ID);
	}
}

void WireTile::calculateChanges(Level* level, int x, int y, int z, int x2, int y2, int z2)
{
	int oldPower = level->getData(x, y, z);
	int newPower = 0;

	m_bIsPowerSource = false;
	bool flag = level->hasNeighborSignal(x, y, z);
	m_bIsPowerSource = true;

	if (flag)
	{
		newPower = 15;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			int checkX = x, checkZ = z;
			if (i == 0) checkX--;
			if (i == 1) checkX++;
			if (i == 2) checkZ--;
			if (i == 3) checkZ++;

			if (checkX != x2 || y != y2 || checkZ != z2)
			{
				newPower = getStrongerSignal(level, checkX, y, checkZ, newPower);
			}

			if (level->isSolidTile(checkX, y, checkZ) && !level->isSolidTile(x, y + 1, z))
			{
				if (checkX != x2 || y + 1 != y2 || checkZ != z2)
				{
					newPower = getStrongerSignal(level, checkX, y + 1, checkZ, newPower);
				}
			}

			if (!level->isSolidTile(checkX, y, checkZ))
			{
				if (checkX != x2 || y - 1 != y2 || checkZ != z2)
				{
					newPower = getStrongerSignal(level, checkX, y - 1, checkZ, newPower);
				}
			}
		}

		if (newPower > 0)
			newPower--;
		else
			newPower = 0;
	}

	if (oldPower != newPower)
	{
		level->field_30 = true;
		level->setData(x, y, z, newPower);
		level->setTilesDirty(x, y, z, x, y, z);
		level->field_30 = false;

		for (int i = 0; i < 4; i++)
		{
			int checkX = x;
			int checkZ = z;
			int checkY = y - 1;
			if (i == 0) checkX--;
			if (i == 1) checkX++;
			if (i == 2) checkZ--;
			if (i == 3) checkZ++;

			if (level->isSolidTile(checkX, y, checkZ))
				checkY += 2;

			int power = 0;
			power = getStrongerSignal(level, checkX, y, checkZ, -1);
			newPower = level->getData(x, y, z);
			if (newPower > 0)
				newPower--;
			
			if (power >= 0 && power != newPower)
				calculateChanges(level, checkX, y, checkZ, x, y, z);
			
			power = getStrongerSignal(level, checkX, checkY, checkZ, -1);
			newPower = level->getData(x, y, z);
			if (newPower > 0)
				newPower--;
			if (power >= 0 && power != newPower)
				calculateChanges(level, checkX, checkY, checkZ, x, y, z);
		}

		if (oldPower == 0 || newPower == 0)
		{
			m_positionsToUpdate.insert(TilePos(x, y, z));
			m_positionsToUpdate.insert(TilePos(x - 1, y, z));
			m_positionsToUpdate.insert(TilePos(x + 1, y, z));
			m_positionsToUpdate.insert(TilePos(x, y - 1, z));
			m_positionsToUpdate.insert(TilePos(x, y + 1, z));
			m_positionsToUpdate.insert(TilePos(x, y, z - 1));
			m_positionsToUpdate.insert(TilePos(x, y, z + 1));
		}
	}
}

int WireTile::getStrongerSignal(Level* level, int x, int y, int z, int prevSignal)
{
	if (level->getTile(x, y, z) != m_ID)
		return prevSignal;

	int newSignal = level->getData(x, y, z);
	return std::max(newSignal, prevSignal);
}

void WireTile::updateWires(Level* level, int x, int y, int z)
{
	if (level->getTile(x, y, z) != m_ID) return;

	level->updateNeighborsAt(x, y, z, m_ID);
	level->updateNeighborsAt(x - 1, y, z, m_ID);
	level->updateNeighborsAt(x + 1, y, z, m_ID);
	level->updateNeighborsAt(x, y, z - 1, m_ID);
	level->updateNeighborsAt(x, y, z + 1, m_ID);
	level->updateNeighborsAt(x, y - 1, z, m_ID);
	level->updateNeighborsAt(x, y + 1, z, m_ID);
}

void WireTile::updateShape(LevelSource* level, int x, int y, int z)
{
	m_aabb = AABB(0.0f, 0.0f, 0.0f, 1.0f, 0.0625f, 1.0f);
}

AABB* WireTile::getAABB(Level* level, int x, int y, int z)
{
	updateShape(level, x, y, z);
	return Tile::getAABB(level, x, y, z);
}

AABB WireTile::getTileAABB(Level* level, int x, int y, int z)
{
	updateShape(level, x, y, z);
	return Tile::getTileAABB(level, x, y, z);
}

void WireTile::addAABBs(Level*, int x, int y, int z, const AABB* aabb, std::vector<AABB>& out)
{
	// there is no collision with redstone!!
}

void WireTile::onPlace(Level* level, int x, int y, int z)
{
	Tile::onPlace(level, x, y, z);
	if (level->m_bIsMultiplayer)
		return;

	recalculate(level, x, y, z);
	level->updateNeighborsAt(x, y + 1, z, m_ID);
	level->updateNeighborsAt(x, y - 1, z, m_ID);
	updateWires(level, x - 1, y, z);
	updateWires(level, x + 1, y, z);
	updateWires(level, x, y, z - 1);
	updateWires(level, x, y, z + 1);

	updateWires(level, x - 1, level->isSolidTile(x - 1, y, z) ? y + 1 : y - 1, z);
	updateWires(level, x + 1, level->isSolidTile(x + 1, y, z) ? y + 1 : y - 1, z);
	updateWires(level, x, level->isSolidTile(x - 1, y, z) ? y + 1 : y - 1, z - 1);
	updateWires(level, x, level->isSolidTile(x + 1, y, z) ? y + 1 : y - 1, z + 1);
}

void WireTile::onRemove(Level* level, int x, int y, int z)
{
	if (level->m_bIsMultiplayer)
	{
		Tile::onRemove(level, x, y, z);
		return;
	}

	recalculate(level, x, y, z);
	level->updateNeighborsAt(x, y + 1, z, m_ID);
	level->updateNeighborsAt(x, y - 1, z, m_ID);
	updateWires(level, x - 1, y, z);
	updateWires(level, x + 1, y, z);
	updateWires(level, x, y, z - 1);
	updateWires(level, x, y, z + 1);

	updateWires(level, x - 1, level->isSolidTile(x - 1, y, z) ? y + 1 : y - 1, z);
	updateWires(level, x + 1, level->isSolidTile(x + 1, y, z) ? y + 1 : y - 1, z);
	updateWires(level, x, level->isSolidTile(x - 1, y, z) ? y + 1 : y - 1, z - 1);
	updateWires(level, x, level->isSolidTile(x + 1, y, z) ? y + 1 : y - 1, z + 1);

	Tile::onRemove(level, x, y, z);
}

void WireTile::neighborChanged(Level* level, int x, int y, int z, int id)
{
	if (level->m_bIsMultiplayer)
		return;

	if (!canSurvive(level, x, y, z))
	{
		spawnResources(level, x, y, z, level->getData(x, y, z));
		level->setTile(x, y, z, TILE_AIR);
	}

	recalculate(level, x, y, z);
	Tile::neighborChanged(level, x, y, z, id);
}

int WireTile::getSignal(LevelSource* level, int x, int y, int z, int dir)
{
	if (!m_bIsPowerSource)
		return 0;

	return getDirectSignal(level, x, y, z, dir);
}

int WireTile::getDirectSignal(LevelSource* level, int x, int y, int z, int dir)
{
	if (!m_bIsPowerSource)
		return 0;

	if (level->getData(x, y, z) == 0)
		return 0;

	if (dir == DIR_YPOS)
		return true;

	bool flag0 = isSignalSource(level, x - 1, y, z) || !level->isSolidTile(x - 1, y, z) && isSignalSource(level, x - 1, y - 1, z);
	bool flag1 = isSignalSource(level, x + 1, y, z) || !level->isSolidTile(x + 1, y, z) && isSignalSource(level, x + 1, y - 1, z);
	bool flag2 = isSignalSource(level, x, y, z - 1) || !level->isSolidTile(x, y, z - 1) && isSignalSource(level, x, y - 1, z - 1);
	bool flag3 = isSignalSource(level, x, y, z + 1) || !level->isSolidTile(x, y, z + 1) && isSignalSource(level, x, y - 1, z + 1);
	if (!level->isSolidTile(x, y + 1, z))
	{
		if (level->isSolidTile(x - 1, y, z) && isSignalSource(level, x - 1, y + 1, z)) flag0 = true;
		if (level->isSolidTile(x + 1, y, z) && isSignalSource(level, x + 1, y + 1, z)) flag1 = true;
		if (level->isSolidTile(x, y, z - 1) && isSignalSource(level, x, y + 1, z - 1)) flag2 = true;
		if (level->isSolidTile(x, y, z + 1) && isSignalSource(level, x, y + 1, z + 1)) flag3 = true;
	}
	if (!flag2 && !flag1 && !flag0 && !flag3 && dir >= 2 && dir <= 5) return true;
	if (dir == 2 && flag2 && !flag0 && !flag1) return true;
	if (dir == 3 && flag3 && !flag0 && !flag1) return true;
	if (dir == 4 && flag0 && !flag2 && !flag3) return true;
	if (dir == 5 && flag1 && !flag2 && !flag3) return true;
	return false;
}

int WireTile::getColor(LevelSource*, int, int, int)
{
	// TODO: Why do I need to swap red, green, and blue?
	// Everything seems to be wrong.  Consider fixing it
	return 0x000080;
}

int WireTile::getRenderShape()
{
	return SHAPE_WIRE;
}

int WireTile::getTickDelay()
{
	return 2;
}
