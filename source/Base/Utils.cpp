/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// note: not an official file name

#include "Utils.hpp"

#ifdef _WIN32

#include <Windows.h>
#include <io.h>
#include <direct.h>

// Why are we not using GetTickCount64()? It's simple -- getTimeMs has the exact same problem as using regular old GetTickCount.
#pragma warning(disable : 28159)

#else

#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#define XPL_ACCESS access
#define XPL_MKDIR(path, mode)  mkdir(path, mode)
#endif

#include "compat/GL.hpp"

int g_TimeSecondsOnInit = 0;

DIR* opendir(const char* name)
{
	size_t len = strlen(name);
	if (len == 0)
		return NULL;

	char buf[1024];
	if (len >= 1024 - 5)
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

	strcpy(de.d_name, dir->findData.cFileName);
	de.d_type = (dir->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;

	return &de;
}

void closedir(DIR* dir)
{
	if (dir->current != INVALID_HANDLE_VALUE)
		FindClose(dir->current);

	free(dir);
}

bool createFolderIfNotExists(const char* pDir)
{
	if (!XPL_ACCESS(pDir, 0))
		return true;

	return XPL_MKDIR(pDir, 0755) == 0;
}

bool DeleteDirectory(const std::string& name, bool unused)
{
	DIR* dir = opendir(name.c_str());
	if (!dir)
		return false;

	char buffer[1024];

	while (true)
	{
		dirent* de = readdir(dir);
		if (!de)
			break;

		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;

		snprintf(buffer, sizeof buffer, "%s/%s", name.c_str(), de->d_name);
		remove(buffer);
	}

	closedir(dir);
	return remove(name.c_str()) == 0;
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
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
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

void sleepMs(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

#endif

void drawArrayVT(GLuint buffer, int count, int stride)
{
	xglBindBuffer(GL_ARRAY_BUFFER, buffer);
	glTexCoordPointer(2, GL_FLOAT, stride, (void*)12);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, nullptr);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawArrayVTC(GLuint buffer, int count, int stride)
{
	xglBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexPointer(3, GL_FLOAT, stride, nullptr);
	glTexCoordPointer(2, GL_FLOAT, stride, (void*)12);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, (void*)20);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

float Max(float a, float b)
{
	if (a < b)
		a = b;
	return a;
}
