/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstdarg>
#include <windowsx.h>

#include "thirdparty/GL/GL.hpp"
#include "compat/KeyCodes.hpp"

#include "client/app/App.hpp"
#include "client/app/NinecraftApp.hpp"

#include "client/player/input/Multitouch.hpp"

#include "AppPlatform_win32.hpp"

LPCTSTR g_WindowClassName = TEXT("MCPEClass");

AppPlatform_win32 g_AppPlatform;
NinecraftApp* g_pApp;

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
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		{
			MouseButtonType buttonType = AppPlatform_win32::GetMouseButtonType(iMsg);
			bool buttonState = AppPlatform_win32::GetMouseButtonState(iMsg, wParam);
			int posX, posY;
			if (iMsg == WM_MOUSEMOVE)
			{
				posX = GET_X_LPARAM(lParam);
				posY = GET_Y_LPARAM(lParam);
			}
			else
			{
				posX = Mouse::getX();
				posY = Mouse::getY();
			}
			Mouse::feed(buttonType, buttonState, posX, posY);
			Multitouch::feed(buttonType, buttonState, posX, posY, 0);
			break;
		}

		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			Minecraft::width  = width;
			Minecraft::height = height;
			Minecraft::setGuiScaleMultiplier(1.0f); // assume no meddling with the DPI stuff

			g_AppPlatform.setScreenSize(width, height);

			if (g_pApp)
				g_pApp->sizeUpdate(width, height);

			break;
		}


		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			Keyboard::KeyState state = AppPlatform_win32::GetKeyState(iMsg);
			Keyboard::feed(state, int(wParam));

			if (wParam == VK_SHIFT)
				g_AppPlatform.setShiftPressed(state == Keyboard::KeyState::DOWN);

			break;
		}

		case WM_CHAR:
		{
			if (lParam & (1 << 31))
				break;

			if (wParam >= '~' && wParam < ' ')
				break;

			g_pApp->handleCharInput(char(wParam));
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return 0;
}

extern bool g_bIsMenuBackgroundAvailable; // client/gui/Screen.cpp
extern bool g_bAreCloudsAvailable;        // client/renderer/LevelRenderer.cpp
extern bool g_bIsGrassColorAvailable;	  // world/level/GrassColor.cpp
extern bool g_bIsFoliageColorAvailable;   // world/level/FoliageColor.cpp

void CheckOptionalTextureAvailability()
{
	// TODO: These should be inside of an initialized "Minecraft" instance rather than the global namespace
	// Optional features that you really should be able to get away with not including.
	g_bIsMenuBackgroundAvailable = XPL_ACCESS("assets/gui/background/panorama_0.png", 0) == 0;
	g_bAreCloudsAvailable        = XPL_ACCESS("assets/environment/clouds.png",        0) == 0;
	g_bIsGrassColorAvailable     = XPL_ACCESS("assets/misc/grasscolor.png",           0) == 0;
	g_bIsFoliageColorAvailable   = XPL_ACCESS("assets/misc/foliagecolor.png",         0) == 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(_DEBUG) && defined(MOD_POPOUT_CONSOLE)
	AllocConsole();
	FILE* ostream;
	FILE* istream;
	freopen_s(&ostream, "CONOUT$", "w", stdout);
	freopen_s(&istream, "CONIN$", "r", stdin);
	SetConsoleTitle("ReMinecraftPE Debug Console");
#endif

	SetInstance(hInstance);

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
	
	CheckOptionalTextureAvailability();

	RECT wr = { 0,0, g_AppPlatform.getScreenWidth(), g_AppPlatform.getScreenHeight() };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
	int w = wr.right - wr.left;
	int h = wr.bottom - wr.top;

	const char* windowTitle = g_AppPlatform.getWindowTitle();
	// Dumb Unicode bullshit
	//LPTSTR windowTitle;
	//mbstowcs(&windowTitle, windowTitleStr, 255);

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Could not register Minecraft class"), windowTitle, MB_ICONERROR | MB_OK);
		return 1;
	}

	HWND hWnd = CreateWindowEx(0, g_WindowClassName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, hInstance, g_pApp);

	CenterWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);
	SetHWND(hWnd);

	HDC hDC; HGLRC hRC;
	// enable OpenGL for the window
	EnableOpenGL(hWnd, &hDC, &hRC);

	xglInit();

	if (!xglInitted())
	{
		const char* const GL_ERROR_MSG = "Error initializing GL extensions. OpenGL 2.0 or later is required. Update your graphics drivers!";
		LOG_E(GL_ERROR_MSG);
		MessageBoxA(GetHWND(), GL_ERROR_MSG, "OpenGL Error", MB_OK);

		goto _cleanup;
	}

	xglSwapIntervalEXT(1);

	g_pApp = new NinecraftApp;
	g_pApp->m_pPlatform = &g_AppPlatform;
	g_pApp->m_externalStorageDir = ".";

	// initialize the app
	g_pApp->init();
	g_pApp->sizeUpdate(Minecraft::width, Minecraft::height);

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

			// note: NinecraftApp would have done this with eglSwapBuffers, but I'd rather do it here:
			SwapBuffers(hDC);
		}
	}

_cleanup:
	g_pApp->saveOptions();

	// disable OpenGL for the window
	DisableOpenGL(hWnd, hDC, hRC);

	// destroy the window explicitly, since we ignored the WM_QUIT message
	DestroyWindow(hWnd);

	hWnd = NULL;
	SetHWND(NULL);

	delete g_pApp;

	return 0;
}
