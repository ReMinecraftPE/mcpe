/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Region.hpp"

TileID Region::getTile(int x, int y, int z)
{
	if (y < C_MIN_Y || y >= C_MAX_Y)
		return TILE_AIR;

	LevelChunk* pChunk = getChunkAt(x, z);

	if (pChunk == nullptr)
		return TILE_AIR;

	return pChunk->getTile(x & 0xF, y, z & 0xF);
}

int Region::getRawBrightness(int x, int y, int z, bool b)
{
	if (x < C_MIN_X || z < C_MIN_X || x >= C_MAX_X || z > C_MAX_Z)
		return 15;

	if (b)
	{
		TileID tile = getTile(x, y, z);
		if (tile == Tile::stoneSlabHalf->m_ID || tile == Tile::farmland->m_ID)
		{
			int result;

			int b1 = getRawBrightness(x, y + 1, z, false);
			int b2 = getRawBrightness(x - 1, y, z, false);
			int b3 = getRawBrightness(x + 1, y, z, false);
			int b4 = getRawBrightness(x, y, z - 1, false);
			int b5 = getRawBrightness(x, y, z + 1, false);

			result = b1;
			if (result < b2) result = b2;
			if (result < b3) result = b3;
			if (result < b4) result = b4;
			if (result < b5) result = b5;

			return result;
		}
	}
	if (y < C_MIN_Y)
	{
		return 0;
	}
	if (y >= C_MAX_Y)
	{
		int bright = 15 - m_pLevel->m_skyDarken;
		if (bright < 0)
			bright = 0;
		return bright;
	}

	//@BUG: Unsanitized input
	return field_C[(x >> 4) - field_4][(z >> 4) - field_8]->getRawBrightness(x & 0xF, y, z & 0xF, m_pLevel->m_skyDarken);
}

int Region::getRawBrightness(int x, int y, int z)
{
	return getRawBrightness(x, y, z, true);
}

float Region::getBrightness(int x, int y, int z)
{
	return m_pLevel->m_pDimension->field_10[getRawBrightness(x, y, z)];
}

int Region::getData(int x, int y, int z)
{
	if (y < C_MIN_Y || y >= C_MAX_Y)
		return 0;

	return field_C[(x >> 4) - field_4][(z >> 4) - field_8]->getData(x & 0xF, y, z & 0xF);
}

Material* Region::getMaterial(int x, int y, int z)
{
	TileID tile = getTile(x, y, z);
	
	if (tile == TILE_AIR)
		return Material::air;

	return Tile::tiles[tile]->m_pMaterial;
}

bool Region::isSolidTile(int x, int y, int z)
{
	Tile* pTile = Tile::tiles[getTile(x, y, z)];

	if (!pTile)
		return false;

	return pTile->isSolidRender();
}

Region::~Region()
{
	for (int i = 0; i < field_14; i++)
		delete[] field_C[i];

	delete[] field_C;
}

Region::Region(Level* level, int x1, int y1, int z1, int x2, int y2, int z2)
{
	m_pLevel = level;
	field_4 = x1 >> 4;
	field_8 = z1 >> 4;
	field_14 = 1 + (x2 >> 4) - (x1 >> 4);
	field_18 = 1 + (z2 >> 4) - (z1 >> 4);

#ifndef ORIGINAL_CODE
	field_C = nullptr;

	assert(field_14 > 0);
	assert(field_18 > 0);

	if (field_14 <= 0 || field_18 <= 0)
		return;
#endif

	field_C = new LevelChunk ** [field_14];

	for (int i = 0; i < field_14; i++)
	{
		field_C[i] = new LevelChunk * [field_18];
	}

	for (int x = field_4; x <= x2 >> 4; x++)
	{
		for (int z = field_8; z <= z2 >> 4; z++)
		{
			field_C[x - field_4][z - field_8] = level->getChunk(x, z);
		}
	}
}
