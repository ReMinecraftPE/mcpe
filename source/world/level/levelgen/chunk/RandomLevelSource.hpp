/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/levelgen/synth/PerlinNoise.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/feature/Feature.hpp"
#include "world/level/levelgen/feature/LargeCaveFeature.hpp"
#include "common/threading/ThreadLocal.hpp"

class RandomLevelSource : public ChunkSource
{
private:
	struct ThreadData;

public:
	RandomLevelSource(Level* level, Dimension* dimension, uint32_t seed);

	LevelChunk* requestChunk(const ChunkPos& pos, LoadMode loadMode) override;
	bool postProcess(ChunkViewSource& chunkViewSource) override;
	void loadChunk(LevelChunk& chunk) override;
	void postProcessMobsAt(TileSource* tileSource, int, int, Random& random) override;

	void getHeights(float* fptr, int a3, int a4, int a5);
	void prepareHeights(LevelChunk* chunk, float* biomeData);
	void buildSurfaces(LevelChunk* chunk);
	
public:
	ThreadLocal<ThreadData> m_threadData;
	bool field_4;
	LargeCaveFeature m_largeCaveFeature;
	int field_9D8[1024];
	ChunkMap_t m_chunks;
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

