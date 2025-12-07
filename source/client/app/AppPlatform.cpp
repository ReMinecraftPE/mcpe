/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <fstream>

#include "thirdparty/stb_image/include/stb_image.h"

#include "AppPlatform.hpp"
#include "common/Logger.hpp"
#include "compat/LegacyCPP.hpp"
#include "AppPlatformListener.hpp"

AppPlatform* AppPlatform::m_singleton = nullptr;

AppPlatform* const AppPlatform::singleton()
{
	return m_singleton;
}

AppPlatform::AppPlatform()
{
	m_singleton = this;
	m_hWND = nullptr;
}

AppPlatform::~AppPlatform()
{

}

void AppPlatform::_tick()
{

}

void AppPlatform::buyGame()
{

}

int AppPlatform::checkLicense()
{
	return 0; // assume no license
}

void AppPlatform::createUserInput()
{

}

void AppPlatform::finish()
{

}

std::string AppPlatform::getDateString(int time)
{
	time_t tt = time;
	struct tm t;
#ifdef _WIN32
	gmtime_s(&t, &tt);
#else
	gmtime_r(&tt, &t);
#endif
    
	// Format String
	char buf[2048];
	strftime(buf, sizeof buf, "%b %d %Y %H:%M:%S", &t);
    
	// Return
	return std::string(buf);
}

// ??? AppPlatform::getOptionStrings()
// To whoever is confused about the above, we moved it to Options::getOptionStrings()

int AppPlatform::getScreenWidth() const
{
	return C_DEFAULT_SCREEN_WIDTH; // default rez of the XPERIA PLAY?
}

int AppPlatform::getScreenHeight() const
{
	return C_DEFAULT_SCREEN_HEIGHT;
}

std::vector<std::string> AppPlatform::getUserInput()
{
	return std::vector<std::string>();
}

int AppPlatform::getUserInputStatus()
{
	return 0;
}

bool AppPlatform::hasBuyButtonWhenInvalidLicense()
{
	return false;
}

// void AppPlatform::loadTexture(const std::string&, bool);

void AppPlatform::saveScreenshot(const std::string&, int, int)
{

}

void AppPlatform::showDialog(eDialogType type)
{

}

void AppPlatform::uploadPlatformDependentData(int, void*)
{

}

void AppPlatform::loadImage(ImageData& data, const std::string& path)
{
	AssetFile file = readAssetFile(path, true);

	if (!file.data)
		return;

	// Parse Image
	int desired_channels = STBI_rgb_alpha; // STB will convert the image for us
	int channels = 0;
	stbi_uc* img = stbi_load_from_memory(file.data, file.size, &data.m_width, &data.m_height, &channels, desired_channels);
	delete[] file.data;
	if (!img)
	{
		// Failed To Parse Image
		LOG_E("The image could not be loaded properly: %s", path.c_str());
		return;
	}

	if (desired_channels == STBI_rgb_alpha)
		channels = STBI_rgb_alpha;

	data.m_data = img;
	data.m_colorSpace = channels == 3 ? COLOR_SPACE_RGB : COLOR_SPACE_RGBA;
}

TextureData AppPlatform::loadTexture(const std::string& path, bool bIsRequired)
{
	TextureData out;
	loadImage(out.m_imageData, path);
	return out;
}

bool AppPlatform::doesTextureExist(const std::string& path) const
{
	return false;
}

bool AppPlatform::isTouchscreen() const
{
	return true;
}

bool AppPlatform::hasGamepad() const
{
	return false;
}

void AppPlatform::recenterMouse()
{

}

void AppPlatform::setMouseGrabbed(bool b)
{

}

void AppPlatform::getMouseDiff(int& x, int& y)
{
	x = y = 0;
}

void AppPlatform::clearDiff()
{
	
}

void AppPlatform::updateFocused(bool focused)
{
}

bool AppPlatform::controlPressed()
{
	return false;
}

bool AppPlatform::shiftPressed()
{
	return false;
}

void AppPlatform::showKeyboard(int x, int y, int w, int h)
{
	showKeyboard();
}

void AppPlatform::showKeyboard()
{
}

void AppPlatform::showKeyboard(bool bShown)
{
	if (bShown)
		showKeyboard();
	else
		hideKeyboard();
}

void AppPlatform::hideKeyboard()
{
}

void AppPlatform::onHideKeyboard()
{
}

#ifdef USE_NATIVE_ANDROID
int AppPlatform::getKeyboardUpOffset()
{
	return 0;
}
#endif

void AppPlatform::vibrate(int milliSeconds)
{
}

bool AppPlatform::getRecenterMouseEveryTick()
{
    return true;
}

std::string AppPlatform::getClipboardText()
{
	return "";
}

void AppPlatform::_fireLowMemory()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->onLowMemory();
	}
}

void AppPlatform::_fireAppSuspended()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->onAppSuspended();
	}
	_fireLowMemory();
}

void AppPlatform::_fireAppResumed()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->onAppResumed();
	}
}

void AppPlatform::_fireAppFocusLost()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->onAppFocusLost();
	}
}

void AppPlatform::_fireAppFocusGained()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->onAppFocusGained();
	}
}

void AppPlatform::_fireAppTerminated()
{
	for (ListenerMap::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		it->second->terminate();
	}
}

bool AppPlatform::hasFileSystemAccess()
{
	return false;
}

std::string AppPlatform::getPatchData()
{
	return readAssetFileStr(_getPatchDataPath(), false);
}

std::string AppPlatform::getAssetPath(const std::string& path) const
{
	std::string realPath = path;
	if (realPath.size() && realPath[0] == '/')
	{
		// trim it off
		realPath = realPath.substr(1);
	}
	realPath = "assets/" + realPath;

	return realPath;
}

AssetFile AppPlatform::readAssetFile(const std::string& path, bool quiet) const
{
	std::string realPath = getAssetPath(path);
	std::ifstream ifs(realPath.c_str(), std::ios::binary);
    
	// Open File
	if (!ifs.is_open())
	{
		if (!quiet) LOG_W("Couldn't find asset file: %s", realPath.c_str());
		return AssetFile();
	}
    
    std::filebuf* pbuf = ifs.rdbuf();
    
	// Get File Size
    std::streamoff size = pbuf->pubseekoff(0, ifs.end, ifs.in);
    pbuf->pubseekpos(0, ifs.in);
	if (size < 0)
	{
		if (!quiet) LOG_E("Error determining the size of the asset file!");
		ifs.close();
		return AssetFile();
	}
    
	// Read Data
	char *buf = new char[size];
    pbuf->sgetn(buf, (std::streamsize)size);
    
	// Close File
    ifs.close();
    
	// Return
	return AssetFile((int64_t)size, (uint8_t*)buf);
}

std::string AppPlatform::readAssetFileStr(const std::string& path, bool quiet) const
{
	AssetFile file = readAssetFile(path, quiet);
	if (!file.data)
		return "";
	std::string out = std::string(file.data, file.data + file.size);
	delete file.data;
	return out;
}

void AppPlatform::initSoundSystem()
{
}

SoundSystem* const AppPlatform::getSoundSystem() const
{
	return nullptr;
}
