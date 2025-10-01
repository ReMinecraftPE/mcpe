/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Level.hpp"

class Region : public LevelSource
{
public:
	TileID getTile(const TilePos& pos) const override;
	int getRawBrightness(const TilePos& pos, bool b) const;
	int getRawBrightness(const TilePos& pos) const;
	float getBrightness(const TilePos& pos) const override;
	TileData getData(const TilePos& pos) const override;
	Material* getMaterial(const TilePos& pos) const override;
	bool isSolidTile(const TilePos& pos) const override;
	BiomeSource* getBiomeSource() const override;

	virtual ~Region();
	Region(const Level* level, const TilePos& min, const TilePos& max);

	// inlined in the original, but I doubt they'd actually copy paste this logic
	LevelChunk* getChunkAt(const ChunkPos& pos) const
	{
		int indexX = pos.x - field_4.x;
		int indexZ = pos.z - field_4.z;

		if (indexX < 0 || indexZ < 0 || indexX >= field_14.x || indexZ >= field_14.z)
			return nullptr;

		return field_C[indexZ * field_14.x + indexX];
	}

private:
	ChunkPos field_4;
	// accesses to the array are performed as follows:
	// (x = 0..m_noEntityRenderFrames, z = 0..m_totalEntities)
	// z * m_noEntityRenderFrames + x
	LevelChunk** field_C;
	const Level* m_pLevel;
	ChunkPos field_14;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
};

