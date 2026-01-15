/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <sstream>
#include <shlobj.h>

#include "AppPlatform_win32.hpp"

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "common/Utils.hpp"
#include "renderer/RenderContextImmediate.hpp"

#if MCE_GFX_API_OGL
#include "renderer/platform/ogl/Extensions.hpp"

// needed for screenshots
#include "thirdparty/GL/GL.hpp"
#endif

#include "thirdparty/stb_image/include/stb_image.h"
#include "thirdparty/stb_image/include/stb_image_write.h"

// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

LPCTSTR g_WindowClassName = TEXT("MCPEClass");

AppPlatform_win32::AppPlatform_win32()
{
	m_cursor = NULL;

#if MCE_GFX_API_OGL
	m_hDC = NULL;
	m_hRC = NULL;
#endif

	m_WindowTitle = "ReMinecraftPE";
	// just assume an 854x480 window for now:
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
	m_UserInputStatus = -1;
	m_DialogType = DLG_NONE;

	m_bHasGraphics = false;
	m_bIsFocused = false;
	m_bGrabbedMouse = false;
	m_bActuallyGrabbedMouse = false;
	m_bWasUnfocused = false;
	m_bShiftPressed = false;

	m_MouseDiffX = 0, m_MouseDiffY = 0;

	m_pSoundSystem = nullptr;
}

AppPlatform_win32::~AppPlatform_win32()
{
	SAFE_DELETE(m_pSoundSystem);
}

void AppPlatform_win32::initSoundSystem()
{
	if (m_pSoundSystem)
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
		return;
	}

	LOG_I("Initializing " STR(SOUND_SYSTEM) "...");
	m_pSoundSystem = new SOUND_SYSTEM();
}

int AppPlatform_win32::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_win32::buyGame()
{
	MessageBoxA(_getHWND(), "Buying the game!", getWindowTitle(), MB_OK | MB_ICONINFORMATION);
}

void AppPlatform_win32::saveScreenshot(const std::string& fileName, int width, int height)
{
#if MCE_GFX_API_OGL
	int npixels = width * height;
	uint32_t* pixels = new uint32_t[npixels];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);

	// Verify if the folder exists for saving screenshots and
	// create it if it doesn't 
	// Kinda inefficient but I didn't want to be too intrusive 
	// -Vruk
	// https://stackoverflow.com/a/22182041

	// https://stackoverflow.com/a/8901001
	CHAR mypicturespath[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, mypicturespath);

	static char str[MAX_PATH];

	if (result == S_OK)
		sprintf(str, "%s\\%s", mypicturespath, "MCPE");
	else
		sprintf(str, "%s\\%s", ".", "Screenshots");

	// https://stackoverflow.com/a/8233867
	DWORD error = GetLastError();
	if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND || error == ERROR_INVALID_NAME)
	{
		// https://stackoverflow.com/a/22182041
		CreateDirectoryA(str, NULL);
	}
	
	char fullpath[MAX_PATH];
	sprintf(fullpath, "%s\\%s", str, fileName.c_str());

	stbi_write_png(fullpath, width, height, 4, pixels, width * 4);

	delete[] pixels;
#endif
}

void AppPlatform_win32::createUserInput()
{
	m_UserInput.clear();
	m_UserInputStatus = -1;

	switch (m_DialogType)
	{
		case DLG_CREATE_WORLD:
		{
			// some placeholder for now
			m_UserInput.push_back("New World");
			m_UserInput.push_back("123456");
			m_UserInputStatus = 1;
			break;
		}
		default:
			break;
	}
}

void AppPlatform_win32::showDialog(eDialogType type)
{
	m_DialogType = type;
}

std::string AppPlatform_win32::getDateString(int time)
{
	time_t tt = time;
	struct tm t;
	// using the _s variant. For a different platform there's gmtime_r. This is not directly portable however.
	gmtime_s(&t, &tt);

	//format it with strftime
	char buf[2048];
	strftime(buf, sizeof buf, "%b %d %Y %H:%M:%S", &t);
	//strftime(buf, sizeof buf, "%a %b %d %H:%M:%S %Z %Y", &t);

	return std::string(buf);
}

bool AppPlatform_win32::doesTextureExist(const std::string& path) const
{
	// Get Full Path
	std::string realPath = getAssetPath(path);

	return XPL_ACCESS(realPath.c_str(), 0) == 0;
}

bool AppPlatform_win32::isTouchscreen() const
{
	return false;
}

bool AppPlatform_win32::hasFileSystemAccess()
{
	return true;
}

AssetFile AppPlatform_win32::readAssetFile(const std::string& str, bool quiet, const std::vector<std::string>& resourcepacks) const
{
	std::string path = getAssetPath(str, resourcepacks);
	std::ifstream ifs(path, std::ios::binary | std::ios::ate);
	if (!ifs.is_open())
		return AssetFile();

	std::streamsize size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	unsigned char* buffer = new unsigned char[size];
	ifs.read((char*) buffer, size);

	return AssetFile(size, buffer);
}

void AppPlatform_win32::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;

	if (m_bHasGraphics)
		createWindowSizeDependentResources(Vec2(width, height), Vec2::ONE);
}

void AppPlatform_win32::recenterMouse()
{
	// only recenter the mouse if it's grabbed
	if (!m_bGrabbedMouse)
		return;

	// If we aren't the foreground window, return
	if (GetForegroundWindow() != _getHWND())
	{
		m_bWasUnfocused = true;
		return;
	}

	POINT oldPos = { 0, 0 };
	GetCursorPos(&oldPos);

	/* We're doing this for FUN???
	RECT rect;
	GetClientRect(_getHWND(), &rect);*/

	POINT offs = { getScreenWidth() / 2, getScreenHeight() / 2 };
	ClientToScreen(_getHWND(), &offs);

	SetCursorPos(offs.x, offs.y);

	// Note. The only reason we do it this way instead of
	// using the Mouse class is because, after SetCursorPos,
	// we'll get an event on our window telling us "hey, the
	// user has moved their cursor back to the center! Move
	// the camera back as well", causing a camera that just
	// refuses to move

	// If we were unfocused last frame, ignore the diff data we have.
	if (!m_bWasUnfocused)
	{
		m_MouseDiffX -= offs.x - oldPos.x;
		m_MouseDiffY -= offs.y - oldPos.y;
	}

	m_bWasUnfocused = false;
}

void AppPlatform_win32::setMouseGrabbed(bool b)
{
	m_bGrabbedMouse = b;

	if (m_bActuallyGrabbedMouse == (b && m_bIsFocused))
		return;

	if (!b || !m_bIsFocused)
	{
		m_bActuallyGrabbedMouse = false;

		//show the cursor
		ShowCursor(TRUE);

		//unconfine it
		ClipCursor(NULL);

		clearDiff();
	}
	else
	{
		m_bActuallyGrabbedMouse = true;

		//hide the cursor
		ShowCursor(FALSE);

		//confine it to our client area
		RECT rect;
		GetClientRect(_getHWND(), &rect);

		POINT offs = { 0, 0 };
		ClientToScreen(_getHWND(), &offs);
		rect.left   += offs.x;
		rect.top    += offs.y;
		rect.right  += offs.x;
		rect.bottom += offs.y;

		ClipCursor(&rect);

		// set the cursor pos to the middle of the screen
		recenterMouse();

		clearDiff();
	}
}

void AppPlatform_win32::getMouseDiff(int& x, int& y)
{
	x = m_MouseDiffX;
	y = m_MouseDiffY;
}

void AppPlatform_win32::clearDiff()
{
	m_MouseDiffX = m_MouseDiffY = 0;
}

void AppPlatform_win32::updateFocused(bool focused)
{
	m_bIsFocused = focused;
	setMouseGrabbed(m_bGrabbedMouse);
}

std::string AppPlatform_win32::getClipboardText()
{
	if (!OpenClipboard(nullptr))
		return "";

	HANDLE h = GetClipboardData(CF_TEXT);
	if (!h)
		return "";

	GlobalLock(h);
	std::string text((char*)h);
	GlobalUnlock(h);

	CloseClipboard();

	return text;
}

HWND AppPlatform_win32::createWindow(HINSTANCE hInstance, WNDPROC wndProc, LPVOID lpParam, WORD iconId)
{
	UINT wStyle = 0x0;

#if MCE_GFX_API_OGL
	wStyle |= CS_OWNDC;
#endif

	WNDCLASS wc;
	wc.style = wStyle;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(iconId));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_WindowClassName;

	RECT wr = { 0,0, getScreenWidth(), getScreenHeight() };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
	int w = wr.right - wr.left;
	int h = wr.bottom - wr.top;

	const char* windowTitle = getWindowTitle();

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Could not register Minecraft class"), windowTitle, MB_ICONERROR | MB_OK);
		return NULL;
	}

	return CreateWindowEx(0, g_WindowClassName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, hInstance, lpParam);
}

void AppPlatform_win32::initializeWindow(HWND hWnd, int nCmdShow)
{
	m_hWnd = hWnd;

	centerWindow();
	ShowWindow(hWnd, nCmdShow);
	enableGraphics(hWnd);
}

void AppPlatform_win32::destroyWindow(HWND hWnd)
{
	DestroyWindow(hWnd);
}

void AppPlatform_win32::centerWindow(HWND hWnd)
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

void AppPlatform_win32::enableGraphics(HWND hWnd)
{
#if MCE_GFX_API_OGL
	DWORD flags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
#if MCE_GFX_API_OGL
	flags |= PFD_SUPPORT_OPENGL;
#endif

	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	/* get the device context (DC) */
	m_hDC = GetDC(hWnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = flags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(m_hDC, &pfd);

	BOOL success = SetPixelFormat(m_hDC, iFormat, &pfd);
	assert(success);
	(void)success; /* silence unused variable warn */

	/* create and enable the render context (RC) */
	m_hRC = wglCreateContext(m_hDC);

	wglMakeCurrent(m_hDC, m_hRC);
#endif
}

void AppPlatform_win32::disableGraphics(HWND hWnd)
{
#if MCE_GFX_API_OGL
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);

	ReleaseDC(hWnd, m_hDC);
#endif
}

bool AppPlatform_win32::initGraphics()
{
#if MCE_GFX_API_OGL
	if (!mce::Platform::OGL::InitBindings())
	{
		const char* const GL_ERROR_MSG = "Error initializing GL extensions. OpenGL 2.0 or later is required. Update your graphics drivers!";
		LOG_E(GL_ERROR_MSG);
		MessageBoxA((HWND)m_hWnd, GL_ERROR_MSG, "OpenGL Error", MB_OK);

		return false;
	}

	xglSwapIntervalEXT(1);
#endif

	m_bHasGraphics = true;
	return true;
}

void AppPlatform_win32::createWindowSizeDependentResources(const Vec2& logicalSize, const Vec2& compositionScale)
{
#if MCE_GFX_API_D3D11
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.createWindowSizeDependentResources((HWND)m_hWnd, logicalSize, compositionScale);
#endif
}

void AppPlatform_win32::swapBuffers()
{
#if MCE_GFX_API_OGL
	SwapBuffers(m_hDC);
#endif

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.swapBuffers();
}

MouseButtonType AppPlatform_win32::GetMouseButtonType(UINT iMsg)
{
	switch (iMsg)
	{
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		return BUTTON_LEFT;
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		return BUTTON_RIGHT;
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN:
		return BUTTON_MIDDLE;
	case WM_MOUSEWHEEL:
		return BUTTON_SCROLLWHEEL;
	default:
		return BUTTON_NONE;
	}
}

bool AppPlatform_win32::GetMouseButtonState(UINT iMsg, WPARAM wParam)
{
	bool result;

	switch (iMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		result = true;
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0)
		{
			// "A positive value indicates that the wheel was rotated forward, away from the user."
			result = false;
		}
		else
		{
			// "A negative value indicates that the wheel was rotated backward, toward the user."
			result = true;
		}
		break;
	}
	default:
		result = false;
		break;
	}

	return result;
}

Keyboard::KeyState AppPlatform_win32::GetKeyState(UINT iMsg)
{
	switch (iMsg)
	{
	case WM_KEYUP:
		return Keyboard::KeyState::UP;
	case WM_KEYDOWN:
	default:
		return Keyboard::KeyState::DOWN;
	}
}
