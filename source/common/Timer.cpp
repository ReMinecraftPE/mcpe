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

void Timer::advanceTime(bool paused)
{
	double timeS = getTimeS();
	double timeMs = timeS * 1000.0;// = getAccurateTimeMs();
	if (timeMs - m_lastSyncTime <= 1000)
	{
		if (timeMs - m_lastSyncTime < 0)
		{
			m_lastSyncTime = m_unprocessedTime = timeMs;
		}
	}
	else
	{
		double diff1, diff2;
		diff1 = timeMs - m_lastSyncTime;
		diff2 = timeMs - m_unprocessedTime;
		m_tickAdjustment += ((diff1 / diff2) - m_tickAdjustment) * 0.2f;
	}

	m_deltaTime = timeS - m_lastUpdateTime;
	m_lastUpdateTime = timeS;

	float x1 = m_deltaTime * m_tickAdjustment;
	if (x1 > 1) x1 = 1;
	if (x1 < 0) x1 = 0;

	float x2 = m_partialTicks + x1 * m_timerSpeed * m_ticksPerSecond;
	m_ticks = int(x2);
	m_partialTicks = x2 - m_ticks;
	if (!paused)
		m_renderTicks = m_partialTicks;
	if (m_ticks > 10)
		m_ticks = 10;
}

Timer::Timer()
{
	m_lastUpdateTime = 0;
	m_lastSyncTime = 0;
	m_unprocessedTime = 0;
	m_deltaTime = 0;
	m_tickAdjustment = 1.0f;
	m_ticksPerSecond = 20.0f;
	m_ticks = 0;
	m_renderTicks = 0;
	m_timerSpeed = 1.0f;
	m_partialTicks = 0;

	m_lastSyncTime = m_unprocessedTime = getTimeMs();
}
