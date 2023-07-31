/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Timer.hpp"
#include "Utils.hpp"

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
	if (timeMs - field_4 <= 1000)
	{
		if (timeMs - field_4 < 0)
		{
			field_4 = field_8 = timeMs;
		}
	}
	else
	{
#ifdef USE_ACCURATE_TIMER
		double diff1 = timeMs - field_4;
		double diff2 = timeMs - field_8;
#else
		int diff1 = timeMs - field_4;
		int diff2 = timeMs - field_8;
#endif
		field_C += ((float(diff1) / float(diff2)) - field_C) * 0.2f;
	}

	float diff = float(timeMs) / 1000.0f - field_0;
	field_0 = float(timeMs) / 1000.0f;

	float x1 = diff * field_C;
	if (x1 > 1) x1 = 1;
	if (x1 < 0) x1 = 0;

	float x2 = field_20 + x1 * field_1C * field_10;
	field_14 = int(x2);
	field_20 = x2 - field_14;
	field_18 = x2 - field_14;
	if (field_14 > 10)
		field_14 = 10;
}

Timer::Timer()
{
	field_4 = field_8 = getTimeMs();
}
