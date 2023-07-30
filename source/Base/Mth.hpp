/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cmath>

#include "Random.hpp"

class Mth
{
	static Random g_Random;

public:
	static float Max(float, float);
	static int Max(int, int);
	static float Min(float, float);
	static int Min(int, int);
	static float abs(float);
	static int abs(int);
	static float absMax(float, float);
	static float absMaxSigned(float, float);
	static float atan(float);
	static float atan2(float y, float x);
	static float cos(float);
	static int floor(float);
	static void initMth();
	static int intFloorDiv(int, int);
	static float invSqrt(float);
	static int random(int);
	static float random(void);
	static float sin(float);
	static float sqrt(float);
};

