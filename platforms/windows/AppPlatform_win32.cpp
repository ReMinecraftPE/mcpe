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

#include "GameMods.hpp"

#include "AppPlatform_win32.hpp"
#include "LoggerWin32.hpp"

#include "thirdparty/GL/GL.hpp"

#include "thirdparty/stb_image/include/stb_image.h"
#include "thirdparty/stb_image/include/stb_image_write.h"

AppPlatform_win32::AppPlatform_win32()
{
	m_WindowTitle = "ReMinecraftPE";
	// just assume an 854x480 window for now:
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
	m_UserInputStatus = -1;

	m_bIsFocused = false;
	m_bGrabbedMouse = false;
	m_bActuallyGrabbedMouse = false;
	m_bWasUnfocused = false;
	m_bShiftPressed = false;

	m_MouseDiffX = 0, m_MouseDiffY = 0;

	// This initializes the Logger singleton to use the Windows-specific variant
	// If we didn't initialize it here, the Minecraft class would have our back
	m_pLogger = new LoggerWin32();
	m_pSoundSystem = nullptr;
}

AppPlatform_win32::~AppPlatform_win32()
{
	SAFE_DELETE(m_pSoundSystem);

	// DELETE THIS LAST
	SAFE_DELETE(m_pLogger);
}

void AppPlatform_win32::initSoundSystem()
{
	if (!m_pSoundSystem)
		m_pSoundSystem = new SoundSystemDS();
	else
		LOG_E("Trying to initialize SoundSystem more than once!");
}

int AppPlatform_win32::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_win32::buyGame()
{
	MessageBoxA(GetHWND(), "Buying the game!", getWindowTitle(), MB_OK | MB_ICONINFORMATION);
}

void AppPlatform_win32::saveScreenshot(const std::string& fileName, int width, int height)
{
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
	DWORD ftyp = GetFileAttributesA(str);

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

Texture AppPlatform_win32::loadTexture(const std::string& str, bool bIsRequired)
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	realPath = "assets/" + realPath;

	FILE* f = fopen(realPath.c_str(), "rb");
	if (!f)
	{
		LOG_E("File %s couldn't be opened", realPath.c_str());

	_error:
		if (!bIsRequired)
			return Texture(0, 0, nullptr, 1, 0);

		const std::string msg = "Error loading " + realPath + ". Did you unzip the Minecraft assets?\n\nNote, you will be warned for every missing texture.";
		MessageBoxA(GetHWND(), msg.c_str(), getWindowTitle(), MB_OK | MB_ICONERROR);

		if (f)
			fclose(f);

		return Texture(0, 0, nullptr, 0, 0);
	}

	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_file(f, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LOG_E("File %s couldn't be loaded via stb_image", realPath.c_str());
		goto _error;
	}

	uint32_t* img2 = new uint32_t[width * height];
	memcpy(img2, img, width * height * sizeof(uint32_t));
	stbi_image_free(img);
	img = nullptr;

	fclose(f);
	return Texture(width, height, img2, 1, 0);
}

bool AppPlatform_win32::isTouchscreen()
{
	return false;
}

bool AppPlatform_win32::hasFileSystemAccess()
{
	return true;
}

std::string AppPlatform_win32::getPatchData()
{
	std::ifstream ifs("assets/patches/patch_data.txt");
	if (!ifs.is_open())
		return "";

	std::stringstream ss;
	ss << ifs.rdbuf();

	return ss.str();
}

void AppPlatform_win32::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void AppPlatform_win32::recenterMouse()
{
	// only recenter the mouse if it's grabbed
	if (!m_bGrabbedMouse)
		return;

	// If we aren't the foreground window, return
	if (GetForegroundWindow() != GetHWND())
	{
		m_bWasUnfocused = true;
		return;
	}

	POINT oldPos = { 0, 0 };
	GetCursorPos(&oldPos);

	RECT rect;
	GetClientRect(GetHWND(), &rect);

	POINT offs = { m_ScreenWidth / 2, m_ScreenHeight / 2 };
	ClientToScreen(GetHWND(), &offs);

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
		GetClientRect(GetHWND(), &rect);

		POINT offs = { 0, 0 };
		ClientToScreen(GetHWND(), &offs);
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
