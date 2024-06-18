/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

// CThread - Object oriented pthread wrapper

#if defined(_WIN32)

#ifndef USE_WIN32_THREADS
#if defined(_XBOX) || defined(USE_OLD_CPP) || defined(__MINGW32__)
// USE_WIN32_THREADS - Use a Win32 implementation of threads instead of using pthread
#define USE_WIN32_THREADS
#else
// USE_CPP11_THREADS - Use a C++11 implementation of threads instead of using pthread
#define USE_CPP11_THREADS
#endif
#endif

#else

#define USE_PTHREADS

#endif

#ifdef USE_CPP11_THREADS
// C++11
#include <thread>

#elif defined(USE_WIN32_THREADS)

// win32
#include <stdint.h>

#ifdef _XBOX
#include <xtl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#else

// pthreads
#include <pthread.h>

#endif

typedef void* (*CThreadFunction)(void*);

class CThread
{
public:
	CThread(CThreadFunction, void* param);
	~CThread();

	static void sleep(uint32_t ms);

private:
	CThreadFunction m_func;

#ifdef USE_CPP11_THREADS
	std::thread m_thrd;
#elif defined (USE_WIN32_THREADS)
	HANDLE m_thrd;
#else
	pthread_t m_thrd;
	pthread_attr_t m_thrd_attr;
#endif
};

