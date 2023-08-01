/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstdarg>
#include <windows.h>
#include <WindowsX.h>
//#include <dwmapi.h>
#include <VersionHelpers.h>

#include "compat/GL.hpp"
#include "compat/AKeyCodes.hpp"

#include "App.hpp"
#include "AppPlatform_windows.hpp"
#include "NinecraftApp.hpp"

LPCTSTR g_GameTitle = TEXT("MINECRAFT");
LPCTSTR g_WindowClassName = TEXT("MinecraftClass");

BOOL wantVSync = TRUE;

// windows dwm sync functions
// On windows versions with the DWM compositor enabled, regular opengl vsync isn't accurate to the compositor's display time.
// Waiting until DWM has flushed to display a frame results in a smoother experience.
// If you don't want this, or it causes problems, you can comment out this line.
// The project does not need to be linked against dwmapi.lib and will find the functions if the DLL exists.
#define USE_DWM_SYNC

#ifdef USE_DWM_SYNC
BOOL hasDWM;
typedef HRESULT(WINAPI *DWMFUNC1)(BOOL *pfEnabled);
typedef HRESULT(WINAPI *DWMFUNC2)();
DWMFUNC1 p_DwmIsCompositionEnabled;
DWMFUNC2 p_DwmFlush;

void linkDWM()
{
	HMODULE dwmapiLib = LoadLibrary("dwmapi.dll");

	if (dwmapiLib != NULL)
	{
		printf("Found dwmapi.dll, vsync will sync to compositor.");
		p_DwmIsCompositionEnabled = (DWMFUNC1)GetProcAddress(dwmapiLib, "DwmIsCompositionEnabled");
		p_DwmFlush = (DWMFUNC2)GetProcAddress(dwmapiLib, "DwmFlush");
		hasDWM = true;
	}
	else
	{
		printf("No dwmapi.dll, vsync will sync to monitor.");
		hasDWM = false;
	}
}
#endif

void LogMsg(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);

#ifdef _WIN32
	char buf[10240];
	vsnprintf(buf, sizeof buf, fmt, lst);
	buf[sizeof buf - 1] = 0;

	OutputDebugStringA(buf);
	OutputDebugStringA("\n");
#else
	vfprintf(stderr, fmt, lst);
	fprintf(stderr, "\n");
#endif

	va_end(lst);
}

// I hate duplicating code, but yeah
void LogMsgNoCR(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);

#ifdef _WIN32
	char buf[10240];
	vsnprintf(buf, sizeof buf, fmt, lst);
	buf[sizeof buf - 1] = 0;

	OutputDebugStringA(buf);
#else
	vfprintf(stderr, fmt, lst);
#endif

	va_end(lst);
}

AppPlatform_windows g_AppPlatform;
NinecraftApp* g_pApp;

bool g_LButtonDown, g_RButtonDown;
int g_MousePosX, g_MousePosY;

void UpdateMouse()
{
	Mouse::_x = g_MousePosX;
	Mouse::_y = g_MousePosY;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			g_AppPlatform.updateFocused(iMsg == WM_SETFOCUS);
			break;
		}

		case WM_LBUTTONUP:
		{
			if (g_LButtonDown)
			{
				g_LButtonDown = false;
				UpdateMouse();

				Mouse::feed(1, 0, g_MousePosX, g_MousePosY);
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (!g_LButtonDown)
			{
				g_LButtonDown = true;
				UpdateMouse();

				Mouse::feed(1, 1, g_MousePosX, g_MousePosY);
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			if (g_RButtonDown)
			{
				g_RButtonDown = false;
				UpdateMouse();

				Mouse::feed(2, 0, g_MousePosX, g_MousePosY);
			}
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if (!g_RButtonDown)
			{
				g_RButtonDown = true;
				UpdateMouse();

				Mouse::feed(2, 1, g_MousePosX, g_MousePosY);
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			g_MousePosX = xPos;
			g_MousePosY = yPos;

			UpdateMouse();

			Mouse::feed(0, 0, g_MousePosX, g_MousePosY);

			break;
		}
#ifdef ENH_ALLOW_SCROLL_WHEEL
		case WM_MOUSEWHEEL:
			Mouse::feed(3, GET_WHEEL_DELTA_WPARAM(wParam), g_MousePosX, g_MousePosY);

			break;
#endif
		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			Minecraft::width  = width;
			Minecraft::height = height;

			g_AppPlatform.setScreenSize(width, height);

			break;
		}
		case WM_KEYDOWN:
		{
			Keyboard::feed(1, int(wParam));
			
			if (wParam == VK_SHIFT)
				g_AppPlatform.setShiftPressed(true);

			break;
		}
		case WM_KEYUP:
		{
			Keyboard::feed(0, int(wParam));

			if (wParam == VK_SHIFT)
				g_AppPlatform.setShiftPressed(false);

			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	SetInstance(hInstance);

	g_AppPlatform.initConsts();

#ifdef USE_DWM_SYNC
	linkDWM();
#endif

	// register the window class:
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_WindowClassName;
	
	RECT wr = { 0,0, g_AppPlatform.getScreenWidth(), g_AppPlatform.getScreenHeight() };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
	int w = wr.right - wr.left;
	int h = wr.bottom - wr.top;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Could not register Minecraft class"), g_GameTitle, MB_ICONERROR | MB_OK);
		return 1;
	}

	HWND hWnd = CreateWindowEx(0, g_WindowClassName, g_GameTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, hInstance, g_pApp);

	CenterWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);
	SetHWND(hWnd);

	HDC hDC; HGLRC hRC;
	// enable OpenGL for the window
	EnableOpenGL(hWnd, &hDC, &hRC);

	xglInit();

	if (!xglInitted())
		goto _cleanup;


	g_pApp = new NinecraftApp;
	g_pApp->m_pPlatform = &g_AppPlatform;
	g_pApp->m_externalStorageDir = ".";

	// initialize the app
	g_pApp->init();

	while (!g_pApp->wantToQuit())
	{
		MSG sMsg;
		if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (sMsg.message == WM_QUIT)
			{
				g_pApp->quit();
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessage(&sMsg);
			}
		}
		else
		{
			// update our stuff here:
			g_pApp->update();

			if (wantVSync)
			{
#ifdef USE_DWM_SYNC
				if (hasDWM && IsWindowsVistaOrGreater())
				{
					BOOL enabled = FALSE;

					if (SUCCEEDED(p_DwmIsCompositionEnabled(&enabled)) && enabled)
					{
						xglSwapIntervalEXT(0);
						p_DwmFlush();
					}
				}
				else
#endif
				{
					xglSwapIntervalEXT(1);
				}
			}
			else
			{
				xglSwapIntervalEXT(0);
			}

			// note: NinecraftApp would have done this with eglSwapBuffers, but I'd rather do it here:
			SwapBuffers(hDC);
		}
	}

_cleanup:
	// disable OpenGL for the window
	DisableOpenGL(hWnd, hDC, hRC);

	// destroy the window explicitly, since we ignored the WM_QUIT message
	DestroyWindow(hWnd);

	hWnd = NULL;
	SetHWND(NULL);

	delete g_pApp;

	return 0;
}