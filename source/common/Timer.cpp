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
	if (timeMs - m_lastSyncTime <= 1000)
	{
		if (timeMs - m_lastSyncTime < 0)
		{
			m_lastSyncTime = m_unprocessedTime = timeMs;
		}
	}
	else
	{
#ifdef USE_ACCURATE_TIMER
		double diff1 = timeMs - m_lastSyncTime;
		double diff2 = timeMs - m_unprocessedTime;
#else
		int diff1 = timeMs - m_lastSyncTime;
		int diff2 = timeMs - m_unprocessedTime;
#endif
		m_tickAdjustment += ((float(diff1) / float(diff2)) - m_tickAdjustment) * 0.2f;
	}

	float diff = float(timeMs) / 1000.0f - m_lastUpdateTime;
	m_lastUpdateTime = float(timeMs) / 1000.0f;

	float x1 = diff * m_tickAdjustment;
	if (x1 > 1) x1 = 1;
	if (x1 < 0) x1 = 0;

	float x2 = m_partialTicks + x1 * m_timerSpeed * m_ticksPerSecond;
	m_ticks = int(x2);
	m_partialTicks = x2 - m_ticks;
	m_renderTicks = x2 - m_ticks;
	if (m_ticks > 10)
		m_ticks = 10;
}

Timer::Timer()
{
	m_lastUpdateTime = 0;
#ifndef USE_ACCURATE_TIMER
	m_lastSyncTime = 0;
	m_unprocessedTime = 0;
#else
	m_lastSyncTime = 0;
	m_unprocessedTime = 0;
#endif
	m_tickAdjustment = 1.0f;
	m_ticksPerSecond = 20.0f;
	m_ticks = 0;
	m_renderTicks = 0;
	m_timerSpeed = 1.0f;
	m_partialTicks = 0;

	m_lastSyncTime = m_unprocessedTime = getTimeMs();
}
