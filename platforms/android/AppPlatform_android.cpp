/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <sstream>

#include "AppPlatform_android.hpp"
#include "client/player/input/Mouse.hpp"

#include "thirdparty/stb_image.h"
#include "thirdparty/stb_image_write.h"


AppPlatform_android::AppPlatform_android()
{
	m_UserInputStatus = -1;

	m_bIsFocused = false;
	m_bGrabbedMouse = false;
	m_bActuallyGrabbedMouse = false;
	m_bWasUnfocused = false;
	m_bShiftPressed = false;

	m_MouseDiffX = 0, m_MouseDiffY = 0;
}

void AppPlatform_android::initConsts()
{
	// just assume an 854x480 window for now:
	m_ScreenWidth  = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
}

int AppPlatform_android::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_android::buyGame()
{
}

void AppPlatform_android::saveScreenshot(const std::string& fileName, int width, int height)
{
}

int AppPlatform_android::getScreenWidth() const
{
	return m_ScreenWidth;
}

int AppPlatform_android::getScreenHeight() const
{
	return m_ScreenHeight;
}

std::vector<std::string> AppPlatform_android::getUserInput()
{
	return m_UserInput;
}

int AppPlatform_android::getUserInputStatus()
{
	return m_UserInputStatus;
}

void AppPlatform_android::createUserInput()
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

void AppPlatform_android::showDialog(eDialogType type)
{
	m_DialogType = type;
}

std::string AppPlatform_android::getDateString(int time)
{


	return std::string("fuck your time idiot");
}

Texture AppPlatform_android::loadTexture(const std::string& str, bool b)
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	AAsset* asset = AAssetManager_open(m_app->activity->assetManager, str.c_str(), AASSET_MODE_BUFFER);
	if (!asset) {
		LOG_E("File %s couldn't be opened", realPath.c_str());
	}
	size_t cnt = AAsset_getLength(asset);
	unsigned char* buffer = (unsigned char*)calloc(cnt, sizeof(unsigned char));
	AAsset_read(asset, (void*)buffer, cnt);
	AAsset_close(asset);


	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_memory(buffer, cnt, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LOG_E("File %s couldn't be loaded via stb_image", realPath.c_str());
	}

	free(buffer);
	return Texture(width, height, (uint32_t*)img, 1, 0);
}
/*
std::vector<std::string> AppPlatform_android::getOptionStrings()
{
	std::vector<std::string> o;

	//o.push_back("mp_username");
	//o.push_back("iProgramInCpp");

	std::ifstream ifs("options.txt");
	if (!ifs.is_open())
		return o;

	std::string str;
	while (true)
	{
		if (!std::getline(ifs, str, '\n'))
			break;

		if (str.empty() || str[0] == '#')
			continue;

		std::stringstream ss;
		ss << str;

		std::string key, value;
		if (std::getline(ss, key, '|') && std::getline(ss, value))
		{
			o.push_back(key);
			o.push_back(value);
		}
	}

	return o;
}

void AppPlatform_android::setOptionStrings(const std::vector<std::string>& str)
{
	assert(str.size() % 2 == 0);

	std::ofstream os("options.txt");

	os << "#Config file for Minecraft PE.  The # at the start denotes a comment, removing it makes it a command.\n\n";
	
	for (int i = 0; i < int(str.size()); i += 2)
		os << str[i] << '|' << str[i + 1] << '\n';
}
*/
void AppPlatform_android::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void AppPlatform_android::initAndroidApp(android_app* ptr)
{
	m_app = ptr;
}

void AppPlatform_android::recenterMouse()
{

}

void AppPlatform_android::setMouseGrabbed(bool b)
{
	m_bGrabbedMouse = b;

	if (m_bActuallyGrabbedMouse == (b && m_bIsFocused))
		return;


}

void AppPlatform_android::getMouseDiff(int& x, int& y)
{
	x = m_MouseDiffX;
	y = m_MouseDiffY;
}

void AppPlatform_android::clearDiff()
{
	m_MouseDiffX = m_MouseDiffY = 0;
}

void AppPlatform_android::updateFocused(bool focused)
{
	m_bIsFocused = focused;
	setMouseGrabbed(m_bGrabbedMouse);
}

bool AppPlatform_android::shiftPressed()
{
	return m_bShiftPressed;
}

void AppPlatform_android::setShiftPressed(bool b)
{
	m_bShiftPressed = b;
}