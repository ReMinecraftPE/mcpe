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
	float m_lastUpdateTime;
#ifndef USE_ACCURATE_TIMER
	int m_lastSyncTime;
	int m_unprocessedTime;
#else
	double m_lastSyncTime;
	double m_unprocessedTime;
#endif
	float m_tickAdjustment;
	float m_ticksPerSecond;
	int m_ticks;
	float m_renderTicks;
	float m_timerSpeed;
	float m_partialTicks;
};

