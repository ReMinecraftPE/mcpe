/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Biome.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/level/TilePos.hpp"

class Level;
class BiomeSource
{
private:
	BiomeSource* init();
public:
	BiomeSource() { init(); }
	BiomeSource(Level*);
	virtual ~BiomeSource();

	virtual Biome* getBiome(const ChunkPos& pos);
	virtual Biome** getBiomeBlock(const TilePos& pos, int, int);
	virtual Biome** getBiomeBlock(Biome**, const TilePos& pos, int, int);
	virtual float* getTemperatureBlock(int, int, int, int);

public:
	float* field_4;
	float* field_8;
	float* field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	Biome** field_20;
	PerlinNoise* m_pPerlinNoise[3];
	Random m_Random1;
	Random m_Random2;
	Random m_Random3;
};

