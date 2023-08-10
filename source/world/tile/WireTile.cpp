/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "WireTile.hpp"
#include "world/level/Level.hpp"

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
			tile = Tile::tiles[level->getTile(x + checkXD[i], y + 1, z + checkZD[i])];
			if (tile && tile->isSignalSource())
			{
				connFlags |= (1 << i) | (1 << (i + 4));
				continue;
			}
		}

		// check below:
		if (level->isSolidTile(x + checkXD[i], y, z + checkXD[i]))
			continue;

		tile = Tile::tiles[level->getTile(x + checkXD[i], y - 1, z + checkZD[i])];
		if (tile && tile->isSignalSource())
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

void WireTile::updateShape(LevelSource* level, int x, int y, int z)
{
	int connFlags = getConnections(level, x, y, z);

	// cut off parts of the texture if needed
	float cxn = 0.0f, cxp = 1.0f, czn = 0.0f, czp = 1.0f;
	if (~connFlags & (1 << CONN_XN)) cxn += 5.0f / 16.0f;
	if (~connFlags & (1 << CONN_XP)) cxp -= 5.0f / 16.0f;
	if (~connFlags & (1 << CONN_ZN)) czn += 5.0f / 16.0f;
	if (~connFlags & (1 << CONN_ZP)) czp -= 5.0f / 16.0f;

	m_aabb = AABB(cxn, 0.0f, czn, cxp, 0.1f, czp);
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

int WireTile::getRenderShape()
{
	return SHAPE_WIRE;
}
