/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "AppPlatform_windows.hpp"
#include "Mouse.hpp"

#include "thirdparty/stb_image.h"
#include "thirdparty/stb_image_write.h"

extern LPCTSTR g_GameTitle;

void AppPlatform_windows::initConsts()
{
	// just assume an 854x480 window for now:
	m_ScreenWidth  = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
}

int AppPlatform_windows::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_windows::buyGame()
{
	MessageBox(GetHWND(), TEXT("Buying the game!"), g_GameTitle, MB_OK | MB_ICONINFORMATION);
}

void AppPlatform_windows::saveScreenshot(const std::string& fileName, int width, int height)
{
	int npixels = width * height;
	uint32_t* pixels = new uint32_t[npixels];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);
	stbi_write_png(fileName.c_str(), width, height, 4, pixels, width * 4);

	delete[] pixels;
}

int AppPlatform_windows::getScreenWidth() const
{
	return m_ScreenWidth;
}

int AppPlatform_windows::getScreenHeight() const
{
	return m_ScreenHeight;
}

std::vector<std::string> AppPlatform_windows::getUserInput()
{
	return m_UserInput;
}

int AppPlatform_windows::getUserInputStatus()
{
	return m_UserInputStatus;
}

void AppPlatform_windows::createUserInput()
{
	m_UserInput.clear();
	m_UserInputStatus = -1;
}

void AppPlatform_windows::showDialog(eDialogType type)
{
	// TODO
}

std::string AppPlatform_windows::getDateString(int time)
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

Texture AppPlatform_windows::loadTexture(const std::string& str, bool b)
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	realPath = "assets/" + realPath;

	FILE* f = fopen(realPath.c_str(), "rb");
	if (!f)
	{
		LogMsg("File %s couldn't be opened", realPath.c_str());
		return Texture(0, 0, nullptr, 1, 0);
	}

	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_file(f, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LogMsg("File %s couldn't be loaded via stb_image", realPath.c_str());
		fclose(f);
		return Texture(0, 0, nullptr, 1, 0);
	}

	fclose(f);
	return Texture(width, height, (uint32_t*)img, 1, 0);
}

std::vector<std::string> AppPlatform_windows::getOptionStrings()
{
	std::vector<std::string> o;

	o.push_back("mp_username");
	o.push_back("iProgramInCpp");

	return o;
}

void AppPlatform_windows::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void AppPlatform_windows::recenterMouse()
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

	POINT oldPos{ 0, 0 };
	GetCursorPos(&oldPos);

	RECT rect;
	GetClientRect(GetHWND(), &rect);

	POINT offs { m_ScreenWidth / 2, m_ScreenHeight / 2 };
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

void AppPlatform_windows::setMouseGrabbed(bool b)
{
	// only if stuff has changed do we update
	if (m_bGrabbedMouse == b)
		return;

	m_bGrabbedMouse = b;

	if (!b)
	{
		//show the cursor
		ShowCursor(TRUE);

		//unconfine it
		ClipCursor(NULL);

		clearDiff();
	}
	else
	{
		//hide the cursor
		ShowCursor(FALSE);

		//confine it to our client area
		RECT rect;
		GetClientRect(GetHWND(), &rect);

		POINT offs{ 0, 0 };
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

void AppPlatform_windows::getMouseDiff(int& x, int& y)
{
	x = m_MouseDiffX;
	y = m_MouseDiffY;
}

void AppPlatform_windows::clearDiff()
{
	m_MouseDiffX = m_MouseDiffY = 0;
}

bool AppPlatform_windows::shiftPressed()
{
	return m_bShiftPressed;
}

void AppPlatform_windows::setShiftPressed(bool b)
{
	m_bShiftPressed = b;
}
