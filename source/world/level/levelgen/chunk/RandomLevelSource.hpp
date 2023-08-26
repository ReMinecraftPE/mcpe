/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

// @NOTE: This class appears to reference a mythical "std::hashtable", but I couldn't find any traces of it.
// I doubt they used C++11 (since it came out in 2011), but this is weird...
// We'll use std::unordered_map instead.

#include <map>
#include "ChunkSource.hpp"
#include "common/Utils.hpp"
#include "world/level/levelgen/synth/PerlinNoise.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/feature/Feature.hpp"
#include "world/level/levelgen/feature/LargeCaveFeature.hpp"

class RandomLevelSource : public ChunkSource
{
public:
	RandomLevelSource(Level*, TLong seed, int);
	int tick() override;
	bool shouldSave() override;
	bool hasChunk(int x, int z) override;
	LevelChunk* create(int x, int z) override;
	LevelChunk* getChunk(int x, int z) override;
	LevelChunk* getChunkDontCreate(int x, int z) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, int, int) override;

	float* getHeights(float*, int, int, int, int, int, int);
	void prepareHeights(int x, int z, TileID*, void*, float*);
	void buildSurfaces (int x, int z, TileID*, Biome**);
	

public:
	bool field_4;
	LargeCaveFeature m_largeCaveFeature;
	int field_9D8[1024];
	std::map<int, LevelChunk*> m_chunks;
	float field_19F0;
	Random m_random;
	PerlinNoise m_perlinNoise1;
	PerlinNoise m_perlinNoise2;
	PerlinNoise m_perlinNoise3;
	PerlinNoise m_perlinNoise4;
	PerlinNoise m_perlinNoise5;
	PerlinNoise m_perlinNoise6;
	PerlinNoise m_perlinNoise7;
	PerlinNoise m_perlinNoise8;
	Level* m_pLevel;
	float* field_7280;
	float field_7284[256];
	float field_7684[256];
	float field_7A84[256];
	// @TODO
	float* field_7E84;
	float* field_7E88;
	float* field_7E8C;
	float* field_7E90;
	float* field_7E94;
	// @TODO


	static const float SNOW_CUTOFF;
	static const float SNOW_SCALE;
};

