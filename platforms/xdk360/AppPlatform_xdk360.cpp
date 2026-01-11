#include <fstream>
#include <sstream>

#include "AppPlatform_xdk360.hpp"

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "common/Utils.hpp"
#include "renderer/RenderContextImmediate.hpp"

#include "thirdparty/stb_image/include/stb_image.h"
#include "thirdparty/stb_image/include/stb_image_write.h"

// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

AppPlatform_xdk360::AppPlatform_xdk360()
{
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;

	m_bHasGraphics = false;
	m_bIsFocused = false;
	m_bWasUnfocused = false;

	m_pSoundSystem = nullptr;
}

AppPlatform_xdk360::~AppPlatform_xdk360()
{
	SAFE_DELETE(m_pSoundSystem);
}

void AppPlatform_xdk360::initSoundSystem()
{
	if (m_pSoundSystem)
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
		return;
	}

	LOG_I("Initializing " STR(SOUND_SYSTEM) "...");
	m_pSoundSystem = new SOUND_SYSTEM();
}

int AppPlatform_xdk360::checkLicense()
{
	// we own the game!!
	return 1;
}

std::string AppPlatform_xdk360::getDateString(int time)
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

bool AppPlatform_xdk360::doesTextureExist(const std::string& path) const
{
	// Get Full Path
	std::string realPath = getAssetPath(path);

	return XPL_ACCESS(realPath.c_str(), 0) == 0;
}

bool AppPlatform_xdk360::isTouchscreen() const
{
	return false;
}

bool AppPlatform_xdk360::hasFileSystemAccess()
{
	return true;
}

std::string AppPlatform_xdk360::getAssetPath(const std::string& path) const
{
	std::string assetPath = AppPlatform::getAssetPath(path);
	toDosPath((char*)assetPath.c_str()); // casting to non-const, because fuck you
	return "GAME:\\" + assetPath;
}

void AppPlatform_xdk360::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;

	if (m_bHasGraphics)
		createWindowSizeDependentResources(width, height);
}

void AppPlatform_xdk360::updateFocused(bool focused)
{
	m_bIsFocused = focused;
}

bool AppPlatform_xdk360::initGraphics(unsigned int width, unsigned int height)
{
	m_bHasGraphics = true;

	// D3D9 needs the RenderContext in order to create the D3DDevice using the width and height
	mce::RenderDevice::createInstance();
	setScreenSize(width, height);

	return true;
}

void AppPlatform_xdk360::createWindowSizeDependentResources(unsigned int width, unsigned int height)
{
#if MCE_GFX_API_D3D9
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.createWindowSizeDependentResources((HWND)m_hWnd, width, height);
#endif
}

void AppPlatform_xdk360::swapBuffers()
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.swapBuffers();
}

