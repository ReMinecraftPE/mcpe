/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// note: not an official file name

#include "common/Utils.hpp"
#include "compat/PlatformDefinitions.h"

#if defined(_WIN32) && !defined(_XBOX)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <direct.h>

// Why are we not using GetTickCount64()? It's simple -- getTimeMs has the exact same problem as using regular old GetTickCount.
#pragma warning(disable : 28159)

#elif defined(_XBOX)

#else

#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#endif

// include zlib stuff
// cant get zlib to build on android, they include prebuilt one anyways. using that one
#include "zlib.h"

int g_TimeSecondsOnInit = 0;

#if (!defined(USE_SDL) || defined(_WIN32)) && !defined(ANDROID) && !MC_TARGET_OS_MAC

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

const char* GetTerrainName()
{
	return "terrain.png";
}

const char* GetItemsName()
{
	return "gui/items.png";
}

const char* GetGUIBlocksName()
{
	return "gui/gui_blocks.png";
}

// In regular mode, getTimeMs depends on getTimeS.
// In Win32 mode, it's vice versa. Cool

int getTimeMs();

float getTimeS()
{
#ifdef _WIN32
	return float(getTimeMs()) / 1000.0f;
#else
	// variant implemented by Mojang. This does not work on MSVC
	timeval tv;
	gettimeofday(&tv, NULL);

	if (g_TimeSecondsOnInit == 0)
		g_TimeSecondsOnInit = tv.tv_sec;

	return float(tv.tv_sec - g_TimeSecondsOnInit) + float(tv.tv_usec) / 1000000.0f;
#endif
}

int getTimeMs()
{
#ifdef _WIN32
	// just return GetTickCount
	int time = GetTickCount();

	if (g_TimeSecondsOnInit == 0)
		g_TimeSecondsOnInit = time;

	return time - g_TimeSecondsOnInit;
#else
	return int(getTimeS() * 1000.0f);
#endif
}

time_t getRawTimeS()
{
#ifdef _WIN32
	return time_t(GetTickCount() / 1000);
#else
	timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec;
#endif
}

time_t getEpochTimeS()
{
	return time(0);
}

#ifdef _WIN32

HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;

void SetInstance(HINSTANCE hinst)
{
	g_hInstance = hinst;
}

HINSTANCE GetInstance()
{
	return g_hInstance;
}

void SetHWND(HWND hwnd)
{
	g_hWnd = hwnd;
}

HWND GetHWND()
{
	return g_hWnd;
}

void CenterWindow(HWND hWnd)
{
	RECT r, desk;
	GetWindowRect(hWnd, &r);
	GetWindowRect(GetDesktopWindow(), &desk);

	int wa, ha, wb, hb;

	wa = (r.right - r.left) / 2;
	ha = (r.bottom - r.top) / 2;

	wb = (desk.right - desk.left) / 2;
	hb = (desk.bottom - desk.top) / 2;

	SetWindowPos(hWnd, NULL, wb - wa, hb - ha, r.right - r.left, r.bottom - r.top, 0);
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

#endif

void sleepMs(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

float Max(float a, float b)
{
	if (a < b)
		a = b;
	return a;
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
