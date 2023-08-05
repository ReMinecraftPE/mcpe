/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "BiomeSource.hpp"
#include "Level.hpp"

BiomeSource::BiomeSource()
{
	field_20 = new Biome*[256];
}

BiomeSource::BiomeSource(Level* pLevel) :
	m_Random1(pLevel->getSeed() * 9871),
	m_Random2(pLevel->getSeed() * 39811),
	m_Random3(pLevel->getSeed() * 543321)
{
	m_pPerlinNoise[0] = new PerlinNoise(&m_Random1, 4);
	m_pPerlinNoise[1] = new PerlinNoise(&m_Random2, 4);
	m_pPerlinNoise[2] = new PerlinNoise(&m_Random3, 2);
	field_20 = new Biome*[256];
	field_4 = new float[256];
}

Biome* BiomeSource::getBiome(ChunkPos& pos)
{
	// @BUG: Shifting right by 4 instead of left.  In Java Edition, a shift left by 4 is performed instead.
	return getBiome(pos.x >> 4, pos.z >> 4);
}

Biome* BiomeSource::getBiome(int a, int b)
{
	return *getBiomeBlock(a, b, 1, 1);
}

Biome** BiomeSource::getBiomeBlock(int a, int b, int c, int d)
{
	return getBiomeBlock(field_20, a, b, c, d);
}

Biome** BiomeSource::getBiomeBlock(Biome** pBiomes, int a, int b, int c, int d)
{
	field_4 = m_pPerlinNoise[0]->getRegion(field_4, a, b, c, c, 0.025f, 0.025f, 0.25f);
	field_8 = m_pPerlinNoise[1]->getRegion(field_8, a, b, c, c, 0.05f, 0.05f, 0.3333f);
	field_C = m_pPerlinNoise[2]->getRegion(field_C, a, b, c, c, 0.25f, 0.25f, 0.588f);

	int index = 0;

	for (int i = 0; i < c; i++)
	{
		for (int j = 0; j < d; j++)
		{
			float d = field_C[index] * 1.1f + 0.5f;
			float d1 = 0.01f;
			float d2 = 1.0f - d1;
			float d3 = (field_4[index] * 0.15f + 0.7f) * d2 + d * d1;
			d1 = 0.002f;
			d2 = 1.0f - d1;
			float d4 = (field_8[index] * 0.15f + 0.5f) * d2 + d * d1;
			d3 = 1.0f - (1.0f - d3) * (1.0f - d3);

			if (d3 < 0.0f) d3 = 0.0f;
			if (d4 < 0.0f) d4 = 0.0f;
			if (d3 > 1.0f) d3 = 1.0f;
			if (d4 > 1.0f) d4 = 1.0f;
			
			field_4[index] = d3;
			field_8[index] = d4;
			field_20[index++] = Biome::getBiome(d3, d4);
		}
	}

	return field_20;
}

float* BiomeSource::getTemperatureBlock(int a, int b, int c, int d)
{
	field_4 = m_pPerlinNoise[0]->getRegion(field_4, a, b, c, d, 0.025f, 0.025f, 0.25f);
	field_C = m_pPerlinNoise[2]->getRegion(field_C, a, b, c, d, 0.25f, 0.25f, 0.588f);

	int index = 0;

	for (int i = 0; i < c; i++)
	{
		for (int j = 0; j < d; j++)
		{
			float d = field_C[index] * 1.1f + 0.5f;
			float d1 = 0.01f;
			float d2 = 1.0f - d1;
			float d3 = (field_4[index] * 0.15f + 0.7f) * d2 + d * d1;
			d3 = 1.0f - (1.0f - d3) * (1.0f - d3);
			if (d3 < 0.0f)
				d3 = 0.0f;
			if (d3 > 1.0f)
				d3 = 1.0f;

			field_4[index++] = d3;
		}
	}

	return field_4;
}

BiomeSource::~BiomeSource()
{
	for (int i = 0; i < 3; i++)
		if (m_pPerlinNoise[i])
			delete m_pPerlinNoise[i];

	if (field_4)
		delete[] field_4;

	if (field_8)
		delete[] field_8;

	if (field_C)
		delete[] field_C;

	if (field_20)
		delete[] field_20;
}
