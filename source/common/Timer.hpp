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
	float lastUpdateTime;
#ifndef USE_ACCURATE_TIMER
	int lastSyncTime;
	int unprocessedTime;
#else
	double lastSyncTime;
	double unprocessedTime;
#endif
	float tickAdjustment;
	float ticksPerSecond;
	int ticks;
	float renderTicks;
	float timerSpeed;
	float partialTicks;
};

