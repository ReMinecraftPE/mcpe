/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstdarg>
#include <windowsx.h>

#include "compat/KeyCodes.hpp"
#include "GameMods.hpp"

#include "client/app/App.hpp"
#include "client/app/NinecraftApp.hpp"

#include "client/player/input/Multitouch.hpp"

#include "AppPlatform_win32.hpp"
#include "resource.h"
#include "LoggerWin32.hpp"

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
			Minecraft::setRenderScaleMultiplier(1.0f); // assume no meddling with the DPI stuff

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
			else if (wParam == VK_CONTROL)
				g_AppPlatform.setControlPressed(state == Keyboard::KeyState::DOWN);

			break;
		}

		case WM_CHAR:
		{
			if (lParam & (1 << 31))
				break;

			// CTRL+V corresponds to character 0x16
			if (wParam == '\x16')
			{
				// we are pasting text
				g_pApp->handleTextPaste();
			}
			else
			{
				g_pApp->handleCharInput(char(wParam));
			}

			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return 0;
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

	// This initializes the Logger singleton to use the Windows-specific variant
	// If we didn't initialize it here, the Minecraft class would have our back
	Logger::setSingleton(new LoggerWin32);

	HWND hWnd = g_AppPlatform.createWindow(hInstance, WndProc, g_pApp, IDI_ICON);
	if (!hWnd)
		return 1;
	g_AppPlatform.initializeWindow(hWnd, nCmdShow);

	if (!g_AppPlatform.initGraphics())
		goto _cleanup;

	g_pApp = new NinecraftApp;
	g_pApp->m_pPlatform = &g_AppPlatform;
	g_pApp->m_externalStorageDir = ".";

	// initialize the app
	g_pApp->init();
	g_pApp->sizeUpdate(Minecraft::width, Minecraft::height);
	g_AppPlatform.setScreenSize(Minecraft::width, Minecraft::height);

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
			g_AppPlatform.swapBuffers();
		}
	}

_cleanup:
	g_pApp->saveOptions();

	// Cleanup networking, renderer, sounds, textures, etc.
	delete g_pApp;

	g_AppPlatform.disableGraphics();

	// destroy the window explicitly, since we ignored the WM_QUIT message
	g_AppPlatform.destroyWindow();

	return 0;
}
