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
	TileID getTile(int x, int y, int z) override;
	int getRawBrightness(int x, int y, int z, bool b);
	int getRawBrightness(int x, int y, int z);
	float getBrightness(int x, int y, int z) override;
	int getData(int x, int y, int z) override;
	Material* getMaterial(int x, int y, int z) override;
	bool isSolidTile(int x, int y, int z) override;

	virtual ~Region();
	Region(Level*, int x1, int y1, int z1, int x2, int y2, int z2);

	// inlined in the original, but I doubt they'd actually copy paste this logic
	LevelChunk* getChunkAt(int x, int z)
	{
		int indexX = (x >> 4) - field_4;
		int indexZ = (z >> 4) - field_8;

		if (indexX < 0 || indexZ < 0 || indexX >= field_14 || indexZ >= field_18)
			return nullptr;

		return field_C[indexX][indexZ];
	}

private:
	int field_4;
	int field_8;
	LevelChunk*** field_C;
	Level* m_pLevel;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
};

