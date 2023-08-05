/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

// CThread - Object oriented pthread wrapper
#define USE_CPP11_THREADS

// USE_CPP11_THREADS - Use a C++11 implementation of threads instead of using pthread
#ifdef USE_CPP11_THREADS
#include <thread>
#else
#include <pthread.h>
#endif

typedef void* (*CThreadFunction)(void*);

class CThread
{
public:
	CThread(CThreadFunction, void* parm);
	~CThread();

	static void sleep(uint32_t ms);

private:
	CThreadFunction m_func;

#ifdef USE_CPP11_THREADS
	std::thread m_thrd;
#else
	pthread_t m_thrd;
	pthread_attr_t m_thrd_attr;
#endif
};

