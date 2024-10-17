/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "LocklessTypes.h"

using namespace RakNet;

LocklessUint32_t::LocklessUint32_t()
{
	value=0;
}
LocklessUint32_t::LocklessUint32_t(uint32_t initial)
{
	value=initial;
}
uint32_t LocklessUint32_t::Increment(void)
{
#if defined(_WIN32) && !defined(LOCKLESS_TYPES_USE_MUTEX)
	return (uint32_t) InterlockedIncrement(&value);
#elif defined(ANDROID) || defined(__S3E__) || defined(__APPLE__) || defined(LOCKLESS_TYPES_USE_MUTEX)
	uint32_t v;
	mutex.Lock();
	++value;
	v=value;
	mutex.Unlock();
	return v;
#else
	return __sync_fetch_and_add (&value, (uint32_t) 1);
#endif
}
uint32_t LocklessUint32_t::Decrement(void)
{
#if defined(_WIN32) && !defined(LOCKLESS_TYPES_USE_MUTEX)
	return (uint32_t) InterlockedDecrement(&value);
#elif defined(ANDROID) || defined(__S3E__) || defined(__APPLE__) || defined(LOCKLESS_TYPES_USE_MUTEX)
	uint32_t v;
	mutex.Lock();
	--value;
	v=value;
	mutex.Unlock();
	return v;
#else
	return __sync_fetch_and_add (&value, (uint32_t) -1);
#endif
}
