/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// note: not an official file name

#include "Utils.hpp"

#include <cstring>

#if MC_PLATFORM_WINPC

#include <winsock.h>

// Why are we not using GetTickCount64()? It's simple -- getTimeMs has the exact same problem as using regular old GetTickCount.
#pragma warning(disable : 28159)

#elif MC_PLATFORM_XBOX360

#else

#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#endif

// include zlib stuff
// cant get zlib to build on android, they include prebuilt one anyways. using that one
#include "zlib.h"

int g_TimeSecondsOnInit = 0;

#ifdef _WIN32

DIR* opendir(const char* name)
{
	size_t len = strlen(name);
	if (len == 0)
		return NULL;

	char buf[1024];
	if (len >= sizeof(buf) - 5)
		return NULL;

	strcpy(buf, name);

	if (name[len - 1] != '/')
		strcpy(&buf[len], "/*");
	else
		strcpy(&buf[len], "*");

	DIR* pDir = (DIR*)malloc(sizeof(DIR));
	if (!pDir)
		return pDir;

	memset(pDir, 0, sizeof * pDir);

	// Stupid Unicode bullshit
	//LPTSTR msBuff;
	//mbstowcs(&msBuff, buf, sizeof(buf));

	pDir->current = FindFirstFile(buf, &pDir->findData);
	if (pDir->current == INVALID_HANDLE_VALUE)
	{
		free(pDir);
		return NULL;
	}

	return pDir;
}

dirent* readdir(DIR* dir)
{
	if (dir->current == INVALID_HANDLE_VALUE)
		return NULL;

	static dirent de;

	if (!dir->returnedFirstFileData)
	{
		dir->returnedFirstFileData = true;
	}
	else
	{
		if (!FindNextFile(dir->current, &dir->findData))
			return NULL;
	}

	LPTSTR fileName = dir->findData.cFileName;

	// Stupid Unicode bullshit
	//char* fileName;
	//wcstombs(fileName, fileNameMs, 255);

	strcpy(de.d_name, fileName);
	de.d_type = (dir->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;

	return &de;
}

void closedir(DIR* dir)
{
	if (dir->current != INVALID_HANDLE_VALUE)
		FindClose(dir->current);

	free(dir);
}

#else

#include <sys/types.h>
#include <dirent.h>

#endif

bool createFolderIfNotExists(const char* pDir)
{
	if (!XPL_ACCESS(pDir, 0))
		return true;

	return XPL_MKDIR(pDir, 0755) == 0;
}

bool DeleteDirectory(const std::string& name2, bool unused)
{
	std::string name = name2;
	if (name.empty())
		return false;

	if (name[name.size() - 1] == '/')
		name.resize(name.size() - 1);

	DIR* dir = opendir(name.c_str());
	if (!dir)
		return false;

	while (true)
	{
		dirent* de = readdir(dir);
		if (!de)
			break;

		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;

		const std::string dirPath = name + "/" + de->d_name;
		remove(dirPath.c_str());
	}

	closedir(dir);

#ifdef _WIN32
	return RemoveDirectoryA(name.c_str());
#else
	return remove(name.c_str()) == 0;
#endif
}

#ifdef _WIN32
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	// We're on a version of Windows that's older than XP, it's time to go big or go home...
	// This is a less-accurate way of getting the time we need, but it may work as a failsafe.
	{
		// https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows

		// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
		// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
		// until 00:00:00 January 1, 1970
		static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

		SYSTEMTIME  system_time;
		FILETIME    file_time;
		uint64_t    time;

		GetSystemTime(&system_time);
		SystemTimeToFileTime(&system_time, &file_time); // this can fail. if it does, nothing will tick
		time = ((uint64_t)file_time.dwLowDateTime);
		time += ((uint64_t)file_time.dwHighDateTime) << 32;

		tp->tv_sec = (long)((time - EPOCH) / 10000000L);
		tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
		return 0;
	}
}
#endif

double getTimeS()
{
#ifdef _WIN32
	// Let's shoot our shot, but if we have to change teams mid-game, we're fucked because g_TimeSecondsOnInit won't be set.
	{
		static LARGE_INTEGER Frequency = { 0 };
		static LARGE_INTEGER CounterStart = { 0 }; // a more accurate verison of g_TimeSecondsOnInit specifically for Win32

		// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-large_integer-r1
		// "If your compiler has built-in support for 64-bit integers, use the QuadPart member to store the 64-bit integer. Otherwise, use the LowPart and HighPart members to store the 64-bit integer."
		if (Frequency.QuadPart == 0)
			QueryPerformanceFrequency(&Frequency);

		LARGE_INTEGER liTime;
		if (QueryPerformanceCounter(&liTime))
		{
			if (CounterStart.QuadPart == 0)
				CounterStart = liTime;

			LARGE_INTEGER liElapsedSeconds;
			LARGE_INTEGER liElapsedMicroseconds;
			liElapsedSeconds.QuadPart = liTime.QuadPart - CounterStart.QuadPart;
			
			//
			// We now have the elapsed number of ticks, along with the
			// number of ticks-per-second. We use these values
			// to convert to the number of elapsed microseconds.
			// To guard against loss-of-precision, we convert
			// to microseconds *before* dividing by ticks-per-second.
			//

			liElapsedMicroseconds.QuadPart = (liElapsedSeconds.QuadPart * 100000) / Frequency.QuadPart;

			return double(liElapsedMicroseconds.QuadPart) * (1.0 / 100000.0);
		}
	}
#endif

	// Variant implemented by Mojang. This does not work on MSVC.
	{
		timeval tv;

		gettimeofday(&tv, NULL);

		if (g_TimeSecondsOnInit == 0)
			g_TimeSecondsOnInit = tv.tv_sec;

		return double(tv.tv_sec - g_TimeSecondsOnInit) + double(tv.tv_usec) / 1000000.0;
	}
}

int getTimeMs()
{
	return int(getTimeS() * 1000.0);
}

time_t getRawTimeS()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec;
}

time_t getEpochTimeS()
{
	return time(0);
}

void sleepMs(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

// zlib stuff
uint8_t* ZlibInflateToMemory(uint8_t* pInput, size_t compressedSize, size_t decompressedSize)
{
	int ret;
	z_stream strm;
	memset(&strm, 0, sizeof strm);

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	// initialize the inflation state machine
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return nullptr;

	uint8_t* pDestBuff = new uint8_t[decompressedSize + 1]; //room for extra null at the end;

	pDestBuff[decompressedSize] = 0; //add the extra null, if we decompressed a text file this can be useful
	strm.avail_in = compressedSize;
	strm.next_in = pInput;
	strm.avail_out = decompressedSize;
	strm.next_out = pDestBuff;

	ret = inflate(&strm, Z_NO_FLUSH);
	if (!(ret == Z_OK || ret == Z_STREAM_END))
	{
		SAFE_DELETE_ARRAY(pDestBuff);
		return nullptr;
	}

	(void)inflateEnd(&strm);

	return pDestBuff;
}

uint8_t* ZlibDeflateToMemory(uint8_t* pInput, size_t sizeBytes, size_t* compressedSizeOut)
{
	return ZlibDeflateToMemoryLvl(pInput, sizeBytes, compressedSizeOut, Z_DEFAULT_COMPRESSION);
}

uint8_t* ZlibDeflateToMemoryLvl(uint8_t* pInput, size_t sizeBytes, size_t* compressedSizeOut, int level)
{
	z_stream strm;
	memset(&strm, 0, sizeof strm);
	int ret;

	// initialize deflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	// initialize deflation state machine
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return nullptr;

	// padding bytes in case our compressed output is larger than the raw input for some reason
	const int ZLIB_PADDING_BYTES = (1024 * 5);
	
	uint8_t* pOut = new uint8_t[sizeBytes + ZLIB_PADDING_BYTES];
	strm.avail_in = sizeBytes;
	strm.next_in = pInput;
	strm.avail_out = sizeBytes + ZLIB_PADDING_BYTES;
	strm.next_out = pOut;

	ret = deflate(&strm, Z_FINISH);
	assert(ret != Z_STREAM_ERROR);

	deflateEnd(&strm);
	*compressedSizeOut = strm.total_out;

	return pOut;
}
