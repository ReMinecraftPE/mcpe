/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CThread.hpp"
#include "client/common/Utils.hpp"

#if	   defined(_XBOX)

#elif defined(_WIN32)
#include <Windows.h> // for Sleep()
#else
#include <unistd.h>
#endif

void CThread::sleep(uint32_t ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

CThread::CThread(CThreadFunction func, void* param)
{
	m_func = func;

#ifdef USE_CPP11_THREADS
	std::thread thr(func, param);
	m_thrd.swap(thr);
#elif defined(_XBOX)
	m_thrd = CreateThread(
		NULL, // not used
		0, // initial stack size
		func, // thread function
		param, // thread argument
		0, // creation option
		LPDWORD(1) // thread identifier (but does it really matter if I'm the one managing them...?)
	);
#else
	pthread_attr_init(&m_thrd_attr);
	pthread_attr_setdetachstate(&m_thrd_attr, 1);
	pthread_create(&m_thrd, &m_thrd_attr, m_func, param);
#endif
}

CThread::~CThread()
{
#ifdef USE_CPP11_THREADS
	m_thrd.join();
#elif defined(_XBOX)

#else
	pthread_join(m_thrd, 0);
	pthread_attr_destroy(&m_thrd_attr);
#endif
}
