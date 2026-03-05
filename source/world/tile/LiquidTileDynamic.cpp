/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTileDynamic.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/TileTickingQueue.hpp"

constexpr bool g_bDisableSponges = true; // disable sponges for now

LiquidTileDynamic::LiquidTileDynamic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
}

bool LiquidTileDynamic::checkSpongesNearby(TileSource* source, const TilePos& pos)
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
				if (source->getTile(TilePos(pos.x + ox, pos.y + oy, pos.z + oz)) == Tile::sponge->m_ID)
					return true;
			}
		}
	}

	return false;
}

bool LiquidTileDynamic::isWaterBlocking(TileSource* source, const TilePos& pos)
{
	TileID tile = source->getTile(pos);
	if (tile == Tile::reeds->m_ID)
		return true;

	if (!g_bDisableSponges)
	{
		if (checkSpongesNearby(source, pos))
			return true;
	}

	if (!tile)
		return false;

	return Tile::tiles[tile]->m_pMaterial->isSolid();
}

bool LiquidTileDynamic::canSpreadTo(TileSource* source, const TilePos& pos)
{
	Material* pMtl = source->getMaterial(pos);
	if (pMtl == m_pMaterial || pMtl == Material::lava)
		return false;

	return !isWaterBlocking(source, pos);
}

int LiquidTileDynamic::getSlopeDistance(TileSource* source, const TilePos& pos, int depth, int a7)
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

		if (isWaterBlocking(source, check))
			continue;

		if (source->getMaterial(check) == m_pMaterial &&
			source->getData(check) == 0)
			continue;

		if (!isWaterBlocking(source, TilePos(check.x, check.y - 1, check.z)))
			return depth;

		if (depth >= 4)
			continue;

		int otherCost = getSlopeDistance(source, check, depth + 1, i);
		if (cost > otherCost)
			cost = otherCost;
	}

	return cost;
}

bool* LiquidTileDynamic::getSpread(TileSource* source, const TilePos& pos)
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

		if (isWaterBlocking(source, chk))
			continue;

		if (source->getMaterial(chk) == m_pMaterial &&
			source->getData(chk) == 0)
			continue;

		if (isWaterBlocking(source, TilePos(chk.x, chk.y - 1, chk.z)))
			field_74[i] = getSlopeDistance(source, chk, 1, i);
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

void LiquidTileDynamic::onPlace(TileSource* source, const TilePos& pos)
{
	updateLiquid(source, pos);

	if (source->getTile(pos) == m_ID)
	{
		source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
	}
}

void LiquidTileDynamic::setStatic(TileSource* source, const TilePos& pos)
{
	TileData data = source->getData(pos);
	source->setTileAndDataNoUpdate(pos, FullTile(m_ID + 1, data));
	//level->setTilesDirty(pos, pos);
	//level->sendTileUpdated(pos);
}

void LiquidTileDynamic::trySpreadTo(TileSource* source, const TilePos& pos, TileData data)
{
	if (!canSpreadTo(source, pos))
		return;

	TileID tile = source->getTile(pos);
	if (tile > 0)
	{
		if (m_pMaterial == Material::lava)
		{
			fizz(source, pos);
		}
		else
		{
			Tile::tiles[tile]->spawnResources(source, pos, source->getData(pos));
		}
	}

	source->setTileAndData(pos, FullTile(m_ID, data));
}

// @NOTE: This is inlined in PE.
int LiquidTileDynamic::getSmallestDepth(TileSource* source, const TilePos& pos, int oldDepth)
{
	int depth = getDepth(source, pos);
	if (depth < 0)
		return oldDepth;

	if (depth == 0)
		field_6C++;

	if (depth >= 8)
		depth = 0;

	return oldDepth >= 0 && depth >= oldDepth ? oldDepth : depth;
}

void LiquidTileDynamic::tick(TileSource* source, const TilePos& pos, Random* random)
{
	int depth = getDepth(source, pos);
	int speed;
	if (m_pMaterial != Material::lava || source->getDimensionConst().isWarm())
		speed = 1;
	else
		speed = 2;

	int depthMax, newData;
	bool flag = true;

	if (depth > 0)
	{
		field_6C = 0;

		depthMax = -100;
		depthMax = getSmallestDepth(source, pos.west(), depthMax);
		depthMax = getSmallestDepth(source, pos.east(), depthMax);
		depthMax = getSmallestDepth(source, pos.north(), depthMax);
		depthMax = getSmallestDepth(source, pos.south(), depthMax);

		newData = speed + depthMax;
		if (newData > 7 || newData < 0)
			newData = -1;

		if (getDepth(source, pos.above()) >= 0)
		{
			int depthUp = getDepth(source, pos.above());
			if (depthUp >= 8)
				newData = depthUp;
			else
				newData = depthUp + 8;
		}

		if (field_6C >= 2 && m_pMaterial == Material::water)
		{
			if (source->isSolidBlockingTile(pos.below()))
			{
				newData = 0;
			}
			else if (m_pMaterial == source->getMaterial(pos.below()))
			{
				if (source->getData(pos.below()) == 0)
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
				source->setTile(pos, 0);
			}
			else
			{
				source->setTileAndData(pos, FullTile(m_ID, depth));
				source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
			}
		}
		else if (flag)
		{
			setStatic(source, pos);
		}
	}
	else
	{
		setStatic(source, pos);
	}

	if (canSpreadTo(source, pos.below()))
	{
		if (depth < 8)
			depth += 8;

		source->setTileAndData(pos.below(), FullTile(m_ID, depth));
		return;
	}

	if (depth >= 0 && (depth == 0 || isWaterBlocking(source, pos.below())))
	{
		bool* bSpread = getSpread(source, pos);

		TileData data = depth + speed;
		if (depth >= 8)
			data = 1;

		if (data >= 8)
			return;

		if (bSpread[0]) trySpreadTo(source, pos.west(), data);
		if (bSpread[1]) trySpreadTo(source, pos.east(), data);
		if (bSpread[2]) trySpreadTo(source, pos.north(), data);
		if (bSpread[3]) trySpreadTo(source, pos.south(), data);
	}
}
