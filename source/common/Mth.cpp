/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cmath>
#include <stdint.h>
#include <stdlib.h>

#include "Mth.hpp"

#define C_SIN_TABLE_MULTIPLIER (10430.0f) // (3320.0f * 3.14156f)

#define ANG_TO_SIN_TABLE_INDEX(ang) ((int)  ((ang) * C_SIN_TABLE_MULTIPLIER))
#define SIN_TABLE_INDEX_TO_ANG(ang) ((float)((ang) / C_SIN_TABLE_MULTIPLIER))

float g_SinTable[65536];

Random Mth::g_Random;

void Mth::initMth()
{
	for (int i = 0; i < 65536; i++)
	{
		g_SinTable[i] = sinf(SIN_TABLE_INDEX_TO_ANG(i)); // value is 10430
	}
}

int Mth::intFloorDiv(int a2, int a3)
{
	if (a2 < 0)
		return ~(~a2 / a3);
	
	return a2 / a3;
}

float Mth::invSqrt(float number)
{
	// It looks familiar. With IDA I get a convoluted mess. I'm going to assume
	// they just stole it from Quake.

	int32_t i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( int32_t * ) &y;                   // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );              // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );  // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );  // 2nd iteration, this can be removed

	return y;
}

float Mth::sin(float a2)
{
	int angle = ANG_TO_SIN_TABLE_INDEX(a2) & 0xFFFF;

	return g_SinTable[angle];
}

float Mth::cos(float a2)
{
	int angle = (ANG_TO_SIN_TABLE_INDEX(a2) + 16384) & 0xFFFF;

	return g_SinTable[angle];
}

// ported from 0.6.1
unsigned Mth::fastRandom()
{
	int x0;
	static int  x1, x2, x3, x4;

	x0 = x1;
	x1 = x2;
	x2 = x3;
	x3 = x4;
	return(x4 = x4 ^ (unsigned(x4) >> 19) ^ x0 ^ (x0 << 11) ^ ((x0 ^ unsigned(x0 << 11)) >> 8));
}

float Mth::clamp(float x, float min, float max)
{
	if (x > max)
		return max;
	if (x > min)
		return x;
	else
		return min;
	return max;
}

int Mth::floor(float f)
{
	int result = int(f);

	if (result > f)
		result--;

	return result;
}

float Mth::atan(float f)
{
	return atanf(f);
}

float Mth::atan2(float y, float x)
{
	return atan2f(y, x);
}

float Mth::Min(float a, float b)
{
	return a < b ? a : b;
}

int Mth::Min(int a, int b)
{
	return a < b ? a : b;
}

float Mth::Max(float a, float b)
{
	return a > b ? a : b;
}

int Mth::Max(int a, int b)
{
	return a > b ? a : b;
}

/*float Mth::abs(float f)
{
	if (f < 0.0f)
		f = -f;
	return f;
}*/

int Mth::abs(int d)
{
	if (d < 0)
		d = -d;
	return d;
}

float Mth::absMax(float a2, float a3)
{
	if (a2 < 0.0f)
		a2 = -a2;
	if (a3 < 0.0f)
		a3 = -a3;
	if (a2 <= a3)
		a2 = a3;
	return a2;
}

float Mth::absMaxSigned(float a2, float a3)
{
	if (abs(a2) <= abs(a2))
		a2 = a3;
	return a2;
}

int Mth::random(int max)
{
	return int(g_Random.nextInt(max));
}

float Mth::random()
{
	return g_Random.nextFloat();
}
