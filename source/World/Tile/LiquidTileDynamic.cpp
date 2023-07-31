/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

LiquidTileDynamic::LiquidTileDynamic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
}

bool LiquidTileDynamic::isWaterBlocking(Level* level, int x, int y, int z)
{
	TileID tile = level->getTile(x, y, z);
	if (tile == Tile::reeds->m_ID)
		return true;

	if (!tile)
		return false;

	return Tile::tiles[tile]->m_pMaterial->isSolid();
}

bool LiquidTileDynamic::canSpreadTo(Level* level, int x, int y, int z)
{
	Material* pMtl = level->getMaterial(x, y, z);
	if (pMtl == m_pMaterial || pMtl == Material::lava)
		return false;

	return !isWaterBlocking(level, x, y, z);
}

int LiquidTileDynamic::getSlopeDistance(Level* level, int x, int y, int z, int depth, int a7)
{
	int cost = 1000;
	
	for (int i = 0; i < 4; i++)
	{
		if ((i == 0 && a7 == 1) || (i == 1 && a7 == 0) || (i == 2 && a7 == 3) || (i == 3 && a7 == 2))
			continue;

		int checkX = x, checkY = y, checkZ = z;
		switch (i)
		{
			case 0: checkX--; break;
			case 1: checkX++; break;
			case 2: checkZ--; break;
			case 3: checkZ++; break;
		}

		if (isWaterBlocking(level, checkX, checkY, checkZ))
			continue;

		if (level->getMaterial(checkX, checkY, checkZ) == m_pMaterial &&
			level->getData(checkX,checkY,checkZ) == 0)
			continue;

		if (!isWaterBlocking(level, checkX, checkY - 1, checkZ))
			return depth;

		if (depth >= 4)
			continue;

		int otherCost = getSlopeDistance(level, checkX, checkY, checkZ, depth + 1, i);
		if (cost > otherCost)
			cost = otherCost;
	}

	return cost;
}

bool* LiquidTileDynamic::getSpread(Level* level, int x, int y, int z)
{
	for (int i = 0; i < 4; i++)
	{
		field_74[i] = 1000;

		int xChk = x, zChk = z;

		switch (i)
		{
			case 0:
				xChk--;
				break;
			case 1:
				xChk++;
				break;
			case 2:
				zChk--;
				break;
			case 3:
				zChk++;
				break;
		}

		if (isWaterBlocking(level, xChk, y, zChk))
			continue;

		if (level->getMaterial(xChk, y, zChk) == m_pMaterial ||
			level->getData(xChk, y, zChk) == 0)
			continue;

		if (isWaterBlocking(level, xChk, y - 1, zChk))
			field_74[i] = getSlopeDistance(level, xChk, y, zChk, 1, i);
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

void LiquidTileDynamic::onPlace(Level* level, int x, int y, int z)
{
	updateLiquid(level, x, y, z);

	if (level->getTile(x, y, z) == m_ID)
	{
		level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
	}
}

void LiquidTileDynamic::setStatic(Level* level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	level->setTileAndDataNoUpdate(x, y, z, m_ID + 1, data);
	level->setTilesDirty(x, y, z, x, y, z);
	level->sendTileUpdated(x, y, z);
}

void LiquidTileDynamic::trySpreadTo(Level* level, int x, int y, int z, int data)
{
	if (!canSpreadTo(level, x, y, z))
		return;

	TileID tile = level->getTile(x, y, z);
	if (tile > 0)
	{
		if (m_pMaterial == Material::lava)
		{
			fizz(level, x, y, z);
		}
		else
		{
			Tile::tiles[tile]->spawnResources(level, x, y, z, level->getData(x, y, z));
		}
	}

	level->setTileAndData(x, y, z, m_ID, data);
}

// @NOTE: This is inlined in PE.
int LiquidTileDynamic::getSmallestDepth(Level* level, int x, int y, int z, int oldDepth)
{
	int depth = getDepth(level, x, y, z);
	if (depth < 0)
		return oldDepth;

	if (depth == 0)
		field_6C++;

	if (depth >= 8)
		depth = 0;

	return oldDepth >= 0 && depth >= oldDepth ? oldDepth : depth;
}

void LiquidTileDynamic::tick(Level* level, int x, int y, int z, Random* random)
{
	int depth = getDepth(level, x, y, z);
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
		depthMax = getSmallestDepth(level, x - 1, y, z, depthMax);
		depthMax = getSmallestDepth(level, x + 1, y, z, depthMax);
		depthMax = getSmallestDepth(level, x, y, z - 1, depthMax);
		depthMax = getSmallestDepth(level, x, y, z + 1, depthMax);

		newData = speed + depthMax;
		if (newData > 7 || newData < 0)
			newData = -1;

		if (getDepth(level, x, y + 1, z) >= 0)
		{
			int depthUp = getDepth(level, x, y + 1, z);
			if (depthUp >= 8)
				newData = depthUp;
			else
				newData = depthUp + 8;
		}

		if (field_6C >= 2 && m_pMaterial == Material::water)
		{
			if (level->isSolidTile(x, y - 1, z))
			{
				newData = 0;
			}
			else if (m_pMaterial == level->getMaterial(x, y - 1, z))
			{
				if (!level->getData(x, y - 1, z))
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
				level->setTile(x, y, z, 0);
			}
			else
			{
				level->setData(x, y, z, depth);
				level->addToTickNextTick(x, y, z, m_ID, getTickDelay());
			}
		}
		else if (flag)
		{
			setStatic(level, x, y, z);
		}
	}
	else
	{
		setStatic(level, x, y, z);
	}

	if (canSpreadTo(level, x, y - 1, z))
	{
		if (depth < 8)
			depth += 8;

		level->setTileAndData(x, y - 1, z, m_ID, depth);
		return;
	}

	if (depth >= 0 && (depth == 0 || isWaterBlocking(level, x, y - 1, z)))
	{
		bool* bSpread = getSpread(level, x, y, z);

		int data = depth + speed;
		if (depth >= 8)
			data = 1;

		if (data >= 8)
			return;

		if (bSpread[0]) trySpreadTo(level, x - 1, y, z, data);
		if (bSpread[1]) trySpreadTo(level, x + 1, y, z, data);
		if (bSpread[2]) trySpreadTo(level, x, y, z - 1, data);
		if (bSpread[3]) trySpreadTo(level, x, y, z + 1, data);
	}
}
