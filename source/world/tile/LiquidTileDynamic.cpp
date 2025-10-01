/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTileDynamic.hpp"
#include "world/level/Level.hpp"

constexpr bool g_bDisableSponges = true; // disable sponges for now

LiquidTileDynamic::LiquidTileDynamic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
}

bool LiquidTileDynamic::checkSpongesNearby(Level* level, const TilePos& pos)
{
	// NOTE: I imagine this could get slow. So you can disable it
	if (g_bDisableSponges)
		return false;

	for (int ox = -2; ox <= 2; ox++)
	{
		for (int oy = -2; oy <= 2; oy++)
		{
			for (int oz = -2; oz <= 2; oz++)
			{
				if (level->getTile(TilePos(pos.x + ox, pos.y + oy, pos.z + oz)) == Tile::sponge->m_ID)
					return true;
			}
		}
	}

	return false;
}

bool LiquidTileDynamic::isWaterBlocking(Level* level, const TilePos& pos)
{
	TileID tile = level->getTile(pos);
	if (tile == Tile::reeds->m_ID)
		return true;

	if (!g_bDisableSponges)
	{
		if (checkSpongesNearby(level, pos))
			return true;
	}

	if (!tile)
		return false;

	return Tile::tiles[tile]->m_pMaterial->isSolid();
}

bool LiquidTileDynamic::canSpreadTo(Level* level, const TilePos& pos)
{
	Material* pMtl = level->getMaterial(pos);
	if (pMtl == m_pMaterial || pMtl == Material::lava)
		return false;

	return !isWaterBlocking(level, pos);
}

int LiquidTileDynamic::getSlopeDistance(Level* level, const TilePos& pos, int depth, int a7)
{
	int cost = 1000;
	
	for (int i = 0; i < 4; i++)
	{
		if ((i == 0 && a7 == 1) || (i == 1 && a7 == 0) || (i == 2 && a7 == 3) || (i == 3 && a7 == 2))
			continue;

		TilePos check(pos);
		switch (i)
		{
			case 0: check.x--; break;
			case 1: check.x++; break;
			case 2: check.z--; break;
			case 3: check.z++; break;
		}

		if (isWaterBlocking(level, check))
			continue;

		if (level->getMaterial(check) == m_pMaterial &&
			level->getData(check) == 0)
			continue;

		if (!isWaterBlocking(level, TilePos(check.x, check.y - 1, check.z)))
			return depth;

		if (depth >= 4)
			continue;

		int otherCost = getSlopeDistance(level, check, depth + 1, i);
		if (cost > otherCost)
			cost = otherCost;
	}

	return cost;
}

bool* LiquidTileDynamic::getSpread(Level* level, const TilePos& pos)
{
	for (int i = 0; i < 4; i++)
	{
		field_74[i] = 1000;

		TilePos chk(pos);

		switch (i)
		{
			case 0:
				chk.x--;
				break;
			case 1:
				chk.x++;
				break;
			case 2:
				chk.z--;
				break;
			case 3:
				chk.z++;
				break;
		}

		if (isWaterBlocking(level, chk))
			continue;

		if (level->getMaterial(chk) == m_pMaterial &&
			level->getData(chk) == 0)
			continue;

		if (isWaterBlocking(level, TilePos(chk.x, chk.y - 1, chk.z)))
			field_74[i] = getSlopeDistance(level, chk, 1, i);
		else
			field_74[i] = 0;
	}

	int min = field_74[0];
	// @NOTE: unrolled loop
	for (int i = 1; i < 4; i++)
	{
		if (min >= field_74[i])
			min  = field_74[i];
	}

	// @NOTE: unrolled loop
	for (int i = 0; i < 4; i++)
		field_70[i] = field_74[i] == min;

	return field_70;
}

void LiquidTileDynamic::onPlace(Level* level, const TilePos& pos)
{
	updateLiquid(level, pos);

	if (level->getTile(pos) == m_ID)
	{
		level->addToTickNextTick(pos, m_ID, getTickDelay());
	}
}

void LiquidTileDynamic::setStatic(Level* level, const TilePos& pos)
{
	TileData data = level->getData(pos);
	level->setTileAndDataNoUpdate(pos, m_ID + 1, data);
	level->setTilesDirty(pos, pos);
	level->sendTileUpdated(pos);
}

void LiquidTileDynamic::trySpreadTo(Level* level, const TilePos& pos, TileData data)
{
	if (!canSpreadTo(level, pos))
		return;

	TileID tile = level->getTile(pos);
	if (tile > 0)
	{
		if (m_pMaterial == Material::lava)
		{
			fizz(level, pos);
		}
		else
		{
			Tile::tiles[tile]->spawnResources(level, pos, level->getData(pos));
		}
	}

	level->setTileAndData(pos, m_ID, data);
}

// @NOTE: This is inlined in PE.
int LiquidTileDynamic::getSmallestDepth(Level* level, const TilePos& pos, int oldDepth)
{
	int depth = getDepth(level, pos);
	if (depth < 0)
		return oldDepth;

	if (depth == 0)
		field_6C++;

	if (depth >= 8)
		depth = 0;

	return oldDepth >= 0 && depth >= oldDepth ? oldDepth : depth;
}

void LiquidTileDynamic::tick(Level* level, const TilePos& pos, Random* random)
{
	int depth = getDepth(level, pos);
	int speed;
	if (m_pMaterial != Material::lava || level->m_pDimension->field_D)
		speed = 1;
	else
		speed = 2;

	int depthMax, newData;
	bool flag = true;

	if (depth > 0)
	{
		field_6C = 0;

		depthMax = -100;
		depthMax = getSmallestDepth(level, pos.west(), depthMax);
		depthMax = getSmallestDepth(level, pos.east(), depthMax);
		depthMax = getSmallestDepth(level, pos.north(), depthMax);
		depthMax = getSmallestDepth(level, pos.south(), depthMax);

		newData = speed + depthMax;
		if (newData > 7 || newData < 0)
			newData = -1;

		if (getDepth(level, pos.above()) >= 0)
		{
			int depthUp = getDepth(level, pos.above());
			if (depthUp >= 8)
				newData = depthUp;
			else
				newData = depthUp + 8;
		}

		if (field_6C >= 2 && m_pMaterial == Material::water)
		{
			if (level->isSolidTile(pos.below()))
			{
				newData = 0;
			}
			else if (m_pMaterial == level->getMaterial(pos.below()))
			{
				if (!level->getData(pos.below()))
					newData = 0;
			}
		}

		if (m_pMaterial == Material::lava && newData < 8 && depth < 8 && newData > depth && random->nextInt(4))
		{
			flag = false;
			newData = depth;
		}

		if (newData != depth)
		{
			depth = newData;
			if (depth < 0)
			{
				level->setTile(pos, 0);
			}
			else
			{
				level->setData(pos, depth);
				level->addToTickNextTick(pos, m_ID, getTickDelay());
			}
		}
		else if (flag)
		{
			setStatic(level, pos);
		}
	}
	else
	{
		setStatic(level, pos);
	}

	if (canSpreadTo(level, pos.below()))
	{
		if (depth < 8)
			depth += 8;

		level->setTileAndData(pos.below(), m_ID, depth);
		return;
	}

	if (depth >= 0 && (depth == 0 || isWaterBlocking(level, pos.below())))
	{
		bool* bSpread = getSpread(level, pos);

		TileData data = depth + speed;
		if (depth >= 8)
			data = 1;

		if (data >= 8)
			return;

		if (bSpread[0]) trySpreadTo(level, pos.west(), data);
		if (bSpread[1]) trySpreadTo(level, pos.east(), data);
		if (bSpread[2]) trySpreadTo(level, pos.north(), data);
		if (bSpread[3]) trySpreadTo(level, pos.south(), data);
	}
}
