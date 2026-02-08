/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <stdint.h>
#include "CThread.hpp"
#include "common/Utils.hpp"

#ifdef XENON
static volatile int hardwareThread = 1;
#endif

void CThread::sleep(uint32_t ms)
{
	sleepMs(ms);
}

CThread::CThread(CThreadFunction func, void* context)
{
	m_bJoined = false;
	m_func = func;

#ifdef USE_CPP11_THREADS
	std::thread thr(func, context);
	m_thrd.swap(thr);
#elif defined(USE_WIN32_THREADS)
	DWORD dwThreadId = 0;
	m_thrd = CreateThread(
		NULL, // not used
		0, // initial stack size
		(LPTHREAD_START_ROUTINE)func, // thread function
		context, // thread argument
		0, // creation option
		&dwThreadId // thread identifier (but does it really matter if I'm the one managing them...?)
	);
#elif defined(XENON)
	m_thrd = hardwareThread++;
	xenon_run_thread_task(m_thrd, context, (void*)m_func);

	if (hardwareThread >= 6)
		hardwareThread = 1;
#else
	pthread_attr_init(&m_thrd_attr);
	//pthread_attr_setdetachstate(&m_thrd_attr, 1);
	pthread_create(&m_thrd, &m_thrd_attr, m_func, context);
#endif
}

CThread::~CThread()
{
	join();

#ifdef USE_CPP11_THREADS
#elif defined(USE_WIN32_THREADS)
	CloseHandle(m_thrd);
#elif defined(XENON)
#else
	pthread_attr_destroy(&m_thrd_attr);
#endif
}

void CThread::join()
{
	if (m_bJoined)
		return;

#ifdef USE_CPP11_THREADS
	m_thrd.join();
#elif defined(USE_WIN32_THREADS)
	WaitForSingleObject(m_thrd, INFINITE);
#elif defined(XENON)
	xenon_sleep_thread(m_thrd);
#else
	pthread_join(m_thrd, 0);
#endif

	m_bJoined = true;
}