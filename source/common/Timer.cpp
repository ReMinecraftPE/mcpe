/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Timer.hpp"
#include "common/Utils.hpp"

#if !defined(_WIN32) && defined(USE_ACCURATE_TIMER)
#error "Implement getAccurateTimeMs() for your platform!"
#endif

#if defined(_WIN32) && defined(USE_ACCURATE_TIMER)
static LARGE_INTEGER s_StartTime;
static bool s_Initted;

double getAccurateTimeMs()
{
	// Thanks to @Kleadron for helping out with this!
	if (!s_Initted)
	{
		s_Initted = true;
		QueryPerformanceCounter(&s_StartTime);
	}

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER currentCounter;
	QueryPerformanceCounter(&currentCounter);

	LONGLONG diff = currentCounter.QuadPart - s_StartTime.QuadPart;

	return double(diff) / double(frequency.QuadPart) * 1000.0;
}
#endif

void Timer::advanceTime()
{
#ifdef USE_ACCURATE_TIMER
	double timeMs = getAccurateTimeMs();
#else
	int timeMs = getTimeMs();
#endif
	if (timeMs - lastSyncTime <= 1000)
	{
		if (timeMs - lastSyncTime < 0)
		{
			lastSyncTime = unprocessedTime = timeMs;
		}
	}
	else
	{
#ifdef USE_ACCURATE_TIMER
		double diff1 = timeMs - lastSyncTime;
		double diff2 = timeMs - unprocessedTime;
#else
		int diff1 = timeMs - lastSyncTime;
		int diff2 = timeMs - unprocessedTime;
#endif
		tickAdjustment += ((float(diff1) / float(diff2)) - tickAdjustment) * 0.2f;
	}

	float diff = float(timeMs) / 1000.0f - lastUpdateTime;
	lastUpdateTime = float(timeMs) / 1000.0f;

	float x1 = diff * tickAdjustment;
	if (x1 > 1) x1 = 1;
	if (x1 < 0) x1 = 0;

	float x2 = partialTicks + x1 * timerSpeed * ticksPerSecond;
	ticks = int(x2);
	partialTicks = x2 - ticks;
	renderTicks = x2 - ticks;
	if (ticks > 10)
		ticks = 10;
}

Timer::Timer()
{
	lastUpdateTime = 0;
#ifndef USE_ACCURATE_TIMER
	lastSyncTime = 0;
	unprocessedTime = 0;
#else
	lastSyncTime = 0;
	unprocessedTime = 0;
#endif
	tickAdjustment = 1.0f;
	ticksPerSecond = 20.0f;
	ticks = 0;
	renderTicks = 0;
	timerSpeed = 1.0f;
	partialTicks = 0;

	lastSyncTime = unprocessedTime = getTimeMs();
}
