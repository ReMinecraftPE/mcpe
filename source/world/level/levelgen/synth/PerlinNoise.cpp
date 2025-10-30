/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PerlinNoise.hpp"
#include "common/Logger.hpp"

PerlinNoise::PerlinNoise(int nOctaves)
{
	m_pRandom = &m_random;
	init(nOctaves);
}

PerlinNoise::PerlinNoise(Random* pRandom, int nOctaves)
{
	m_pRandom = pRandom;

	if (nOctaves == 10)
	{
		LOG_I("PerlinNoise octaves are 10");
	}

	init(nOctaves);
}

void PerlinNoise::init(int nOctaves)
{
	m_nOctaves = nOctaves;
	m_pImprovedNoise = new ImprovedNoise * [nOctaves];

	for (int i = 0; i < nOctaves; i++)
	{
		m_pImprovedNoise[i] = new ImprovedNoise(m_pRandom);
	}
}

PerlinNoise::~PerlinNoise()
{
	for (int i = 0; i < m_nOctaves; i++)
		delete[] m_pImprovedNoise[i];

	delete[] m_pImprovedNoise;
}

float PerlinNoise::getValue(float x, float y)
{
	if (m_nOctaves <= 0) return 0.0f;

	float result = 0.0f, x1 = 1.0f;

	for (int i = 0; i < m_nOctaves; i++)
	{
		result += m_pImprovedNoise[i]->getValue(x * x1, y * x1) / x1;
		x1 /= 2.f;
	}

	return result;
}

float PerlinNoise::getValue(float x, float y, float z)
{
	if (m_nOctaves <= 0) return 0.0f;

	float result = 0.0f, x1 = 1.0f;

	for (int i = 0; i < m_nOctaves; i++)
	{
		result += m_pImprovedNoise[i]->getValue(x * x1, y * x1, z * x1) / x1;
		x1 /= 2.f;
	}

	return result;
}

float* PerlinNoise::getRegion(float* a2, int a3, int a4, int a5, int a6, float a7, float a8, float a9)
{
	return getRegion(a2, float(a3), 10.0f, float(a4),  a5, 1, a6,  a7, 1.0f, a8);
}

float* PerlinNoise::getRegion(float* pMem, float a3, float a4, float a5, int a6, int a7, int a8, float a9, float a10, float a11)
{
	int amt = a6 * a7 * a8;
	if (!pMem)
		pMem = new float[amt];

	for (int i = 0; i < amt; i++)
		pMem[i] = 0;

	float x = 1.0f;
	for (int i = 0; i < m_nOctaves; i++)
	{
		m_pImprovedNoise[i]->add(pMem, a3, a4, a5, a6, a7, a8, a9 * x, a10 * x, a11 * x, x);
		x /= 2;
	}

	return pMem;
}
