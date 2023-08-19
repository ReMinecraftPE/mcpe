/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#if defined(_WIN32)
#define USE_ACCURATE_TIMER
#endif

class Timer
{
public:
	Timer();

	void advanceTime();

public:
	float field_0;
#ifndef USE_ACCURATE_TIMER
	int field_4;
	int field_8;
#else
	double field_4;
	double field_8;
#endif
	float field_C;
	float field_10;
	int field_14;
	float field_18;
	float field_1C;
	float field_20;
};

