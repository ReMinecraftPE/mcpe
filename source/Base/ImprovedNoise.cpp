/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ImprovedNoise.hpp"
#include "Mth.hpp"

ImprovedNoise::ImprovedNoise()
{
	Random random(1);
	init(&random);
}

ImprovedNoise::ImprovedNoise(Random* pRandom)
{
	init(pRandom);
}

void ImprovedNoise::init(Random* pRandom)
{
	m_offsetX = pRandom->nextFloat() * 256.0f;
	m_offsetY = pRandom->nextFloat() * 256.0f;
	m_offsetZ = pRandom->nextFloat() * 256.0f;
	
	for (int i = 0; i < 256; i++)
		m_permutation[i] = i;

	for (int i = 0; i < 256; i++)
	{
		int x = pRandom->nextInt(256 - i) + i;
		int t = m_permutation[i];
		m_permutation[i] = m_permutation[x];
		m_permutation[x] = t;
		m_permutation[256 + i] = m_permutation[i];
	}
}

float ImprovedNoise::getValue(float x, float y)
{
	return getValue(x, y, 0.0f);
}

float ImprovedNoise::getValue(float x, float y, float z)
{
	return noise(x, y, z);
}

float ImprovedNoise::lerp(float prog, float a, float b)
{
	return a + (b - a) * prog;
}

float ImprovedNoise::grad(int hash, float x, float y, float z)
{
	int h = hash & 0xF;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float ImprovedNoise::grad2(int hash, float x, float z)
{
	return grad(hash, x, 0.0f, z);
}

float ImprovedNoise::fade(float x)
{
	return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

float ImprovedNoise::noise(float x, float y, float z)
{
	// couldn't figure out how to get it to work well enough so I just decided to port the original implementation from:
	// https://cs.nyu.edu/~perlin/noise/
	x += m_offsetX;
	y += m_offsetY;
	z += m_offsetZ;

	int X = Mth::floor(x) & 255,
		Y = Mth::floor(y) & 255,
		Z = Mth::floor(z) & 255;

	x -= Mth::floor(x);
	y -= Mth::floor(y);
	z -= Mth::floor(z);

	float u = fade(x),
		  v = fade(y),
		  w = fade(z);

	int* p = m_permutation;
	int A = p[X    ] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
		B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
		                           grad(p[BA  ], x-1, y  , z   )),
		                   lerp(u, grad(p[AB  ], x  , y-1, z   ),
							       grad(p[BB  ], x-1, y-1, z   ))),
		           lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
					               grad(p[BA+1], x-1, y  , z-1 )),
					       lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
							       grad(p[BB+1], x-1, y-1, z-1 ))));
}

void ImprovedNoise::add(float* a2, float a3, float a4, float a5, int a6, int a7, int a8, float a9, float a10, float a11, float a12)
{
	// @TODO: clean this up
	if (a7 == 1)
	{
		for (int i = 0; i < a6; i++)
		{
			float x2 = m_offsetX + a9 * (i + a3);
			int   x3 = Mth::floor(x2);
			float x4 = float(x3);
			float x5 = x2 - x4;

			int* x6 = &m_permutation[uint8_t(x3)];
			int* x8 = &m_permutation[uint8_t(x3 + 1)];
			float* x7 = &a2[a8 * i];

			for (int j = 0; j < a8; j++)
			{
				float x9 = m_offsetZ + a11 * (j + a5);
				int   x10 = Mth::floor(x9);
				float x11 = float(x10);
				float x12 = x9 - x11;

				int* x13 = &m_permutation[uint8_t(x10) + m_permutation[*x6]];
				int* x15 = &m_permutation[uint8_t(x10) + m_permutation[*x8]];
				int* x14 = x8;

				float x16 = grad2(*x13, x5, x12);
				float x17 = grad(*x15, x5 - 1, 0, x12);
				float x18 = lerp(fade(x5), x16, x17);
				float x19 = grad(x13[1], x5, 0, x12 - 1);
				float x20 = grad(x15[1], x5 - 1, 0, x12 - 1);
				float x21 = lerp(fade(x5), x19, x20);

				*x7 += (1.0f / a12) * lerp(fade(x12), x18, x21);
				x7++;
			}
		}

		return;
	}

	float x30 = 0, x31 = 0, x32 = 0, x33 = 0;
	int x34 = -1, x35 = 0;

	for (int i = 0; i < a6; i++)
	{
		float x36 = m_offsetX + a9 * (i + a3);
		int   x37 = Mth::floor(x36);
		float x38 = float(x37);
		float x39 = x36 - x38;
		float x40 = fade(x39);
		if (a8 <= 0) continue;

		int* x42 = &m_permutation[uint8_t(x37)];
		int* x43 = &m_permutation[uint8_t(x37) + 1];
		for (int j = 0; j < a8; j++)
		{
			float x44 = m_offsetZ + a11 * (j + a5);
			int   x45 = Mth::floor(x44);
			float x46 = float(x45);
			float x47 = x44 - x46;
			uint8_t x48 = uint8_t(x45);
			if (a7 <= 0) continue;

			float* x49 = &a2[x35];
			for (int k = 0; k < a7; k++)
			{
				float x50 = m_offsetY + a10 * (k + a4);
				int   x51 = Mth::floor(x50);
				float x52 = float(x51);
				float x53 = x50 - x52;
				uint8_t bx51 = uint8_t(x51);

				if (k == 0 || bx51 != x34)
				{
					int* x54 = &m_permutation[bx51 + *x42];
					int  x55 = x54[0] + x48;
					int  x56 = x54[1] + x48;
					int* x57 = &m_permutation[bx51 + *x43];
					int  x58 = x57[1] + x48;
					int* x59 = &m_permutation[*x57 + x48];
					float x60 = grad(m_permutation[x55], x39, x53, x47);
					float x61 = grad(*x59, x39 - 1, x53, x47);
					x33 = lerp(x40, x60, x61);
					float x62 = grad(m_permutation[x56], x39, x53 - 1, x47);
					float x63 = grad(m_permutation[x58], x39 - 1, x53 - 1, x47);
					x32 = lerp(x40, x62, x63);
					float x64 = grad(m_permutation[x55 + 1], x39, x53, x47 - 1);
					float x65 = grad(x59[1], x39 - 1, x53, x47 - 1);
					x31 = lerp(x40, x64, x65);
					float x66 = grad(m_permutation[x56 + 1], x39, x53 - 1, x47 - 1);
					float x67 = grad(m_permutation[x58 + 1], x39 - 1, x53 - 1, x47 - 1);
					x34 = bx51;
					x30 = lerp(x40, x66, x67);
				}

				float x68 = lerp(fade(x53), x33, x32);
				float x69 = lerp(fade(x53), x31, x30);
				*x49 += (1.0f / a12) * lerp(fade(x47), x68, x69);
				x49++;
			}
			x35 += a7;
		}
	}
}
