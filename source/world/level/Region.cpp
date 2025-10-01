/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Region.hpp"

TileID Region::getTile(const TilePos& pos) const
{
	if (pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		return TILE_AIR;

	LevelChunk* pChunk = getChunkAt(pos);

	if (pChunk == nullptr)
		return TILE_AIR;

	return pChunk->getTile(pos);
}

int Region::getRawBrightness(const TilePos& pos, bool b) const
{
	if (pos.x < C_MIN_X || pos.z < C_MIN_X || pos.x >= C_MAX_X || pos.z > C_MAX_Z)
		return 15;

	if (b)
	{
		TileID tile = getTile(pos);
		if (tile == Tile::stoneSlabHalf->m_ID || tile == Tile::farmland->m_ID)
		{
			int result;

			int b1 = getRawBrightness(pos.above(), false);
			int b2 = getRawBrightness(pos.west(), false);
			int b3 = getRawBrightness(pos.east(), false);
			int b4 = getRawBrightness(pos.north(), false);
			int b5 = getRawBrightness(pos.south(), false);

			result = b1;
			if (result < b2) result = b2;
			if (result < b3) result = b3;
			if (result < b4) result = b4;
			if (result < b5) result = b5;

			return result;
		}
	}
	if (pos.y < C_MIN_Y)
	{
		return 0;
	}
	if (pos.y >= C_MAX_Y)
	{
		int bright = 15 - m_pLevel->m_skyDarken;
		if (bright < 0)
			bright = 0;
		return bright;
	}

	//@BUG: Unsanitized input
	ChunkPos d(pos);
	d -= field_4;
	return field_C[d.z * field_14.x + d.x]->getRawBrightness(pos, m_pLevel->m_skyDarken);
}

int Region::getRawBrightness(const TilePos& pos) const
{
	return getRawBrightness(pos, true);
}

float Region::getBrightness(const TilePos& pos) const
{
	return m_pLevel->m_pDimension->field_10[getRawBrightness(pos)];
}

TileData Region::getData(const TilePos& pos) const
{
	if (pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		return 0;

	ChunkPos d(pos);
	d -= field_4;
	return field_C[d.z * field_14.x + d.x]->getData(pos);
}

Material* Region::getMaterial(const TilePos& pos) const
{
	TileID tile = getTile(pos);
	
	if (tile == TILE_AIR)
		return Material::air;

	return Tile::tiles[tile]->m_pMaterial;
}

bool Region::isSolidTile(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];

	if (!pTile)
		return false;

	return pTile->isSolidRender();
}

BiomeSource* Region::getBiomeSource() const
{
	return m_pLevel->getBiomeSource();
}

Region::~Region()
{
	delete[] field_C;
}

Region::Region(const Level* level, const TilePos& min, const TilePos& max)
{
	m_pLevel = level;
	field_4 = min;
	ChunkPos cpMin(min), cpMax(max);
	field_14 = (cpMax - cpMin) + 1;

#ifndef ORIGINAL_CODE
	field_C = nullptr;

	assert(field_14.x > 0);
	assert(field_14.z > 0);

	if (field_14.x <= 0 || field_14.z <= 0)
		return;
#endif

	/*
	m_bFoggy = new LevelChunk ** [m_noEntityRenderFrames.x];

	for (int i = 0; i < m_noEntityRenderFrames.x; i++)
	{
		m_bFoggy[i] = new LevelChunk * [m_noEntityRenderFrames.z];
	}
	*/

	field_C = new LevelChunk * [field_14.x * field_14.z];
	//LOG_I("Region chunk-storage size: %d x %d = %d", m_noEntityRenderFrames.x, m_noEntityRenderFrames.z, m_noEntityRenderFrames.x * m_noEntityRenderFrames.z);

	// NOTE: do NOT compare cp.x with max.x directly, no automatic conversion will occur
	ChunkPos cp(field_4);
	for (cp.x = field_4.x; cp.x <= cpMax.x; cp.x++)
	{
		for (cp.z = field_4.z; cp.z <= cpMax.z; cp.z++)
		{
			int index = (cp.z - field_4.z) * field_14.x + (cp.x - field_4.x);
			//LOG_I("Writing to region chunk-storage at: %d", index);
			field_C[index] = level->getChunk(cp);
		}
	}
}
