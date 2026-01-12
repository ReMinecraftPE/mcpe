#include "AppPlatform_xdk360.hpp"

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "common/Utils.hpp"
#include "renderer/RenderContextImmediate.hpp"

// requiring drive has at least 0 bytes free
#define C_SAVEDEVICE_MINIMUM_FREE_BYTES 0

AppPlatform_xdk360::AppPlatform_xdk360()
{
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;

	m_bHasGraphics = false;
	m_bIsFocused = false;
	m_bWasUnfocused = false;

	m_pSoundSystem = nullptr;

	memset(m_saveDeviceId, 0, C_MAX_LOCAL_PLAYERS);
	m_currentSavingPlayerId = -1;
}

AppPlatform_xdk360::~AppPlatform_xdk360()
{
	SAFE_DELETE(m_pSoundSystem);
}

XCONTENTDEVICEID AppPlatform_xdk360::_getSaveDeviceId(unsigned int playerId)
{
	if (m_saveDeviceId[playerId] != 0)
		return m_saveDeviceId[playerId];

	ULARGE_INTEGER iBytesRequested = { C_SAVEDEVICE_MINIMUM_FREE_BYTES };
	XOVERLAPPED xov = {0};

	DWORD result = XShowDeviceSelectorUI(
		playerId,
		XCONTENTTYPE_SAVEDGAME, XCONTENTFLAG_NONE,
		iBytesRequested,
		&m_saveDeviceId[playerId],
		&xov
	);

	if (result != ERROR_IO_PENDING)
	{
		LOG_W("Failed to open save device for player %d", playerId);
	}

	return m_saveDeviceId[playerId];
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

bool AppPlatform_xdk360::hasGamepad() const
{
	return true;
}

GameControllerHandler* AppPlatform_xdk360::getGameControllerHandler()
{
	return &m_gameControllerHandler;
}

bool AppPlatform_xdk360::hasFileSystemAccess()
{
	return false;
}

std::string AppPlatform_xdk360::getAssetPath(const std::string& path) const
{
	std::string assetPath = AppPlatform::getAssetPath(path);
	toDosPath((char*)assetPath.c_str()); // casting to non-const, because fuck you
	return "GAME:\\" + assetPath;
}

void AppPlatform_xdk360::makeNativePath(std::string& path) const
{
	toDosPath((char*)path.c_str());
}

void AppPlatform_xdk360::beginProfileDataWrite(unsigned int playerId)
{
	if (m_currentSavingPlayerId != -1)
	{
		LOG_E("Tried to begin profile data write for player %d, but player %d is already saving data!", playerId, m_currentSavingPlayerId);
		throw std::bad_cast();
	}

	/*XCONTENT_DATA contentData = {0};
	lstrcpyW(contentData.szDisplayName, L"Profile Data");
	contentData.dwContentType = XCONTENTTYPE_SAVEDGAME;
	contentData.DeviceID = _getSaveDeviceId(playerId);

	// Mount the device to the "savedrive" drive name
	XContentCreate(playerId, "savedrive", &contentData, XCONTENTFLAG_CREATEALWAYS, NULL, NULL, NULL);*/

	m_currentSavingPlayerId = playerId;
}

void AppPlatform_xdk360::endProfileDataWrite(unsigned int playerId)
{
	if (m_currentSavingPlayerId == -1)
	{
		LOG_E("Tried to end profile data write for player %d, but no one is saving any data!", playerId);
		throw std::bad_cast();
	}

	//XContentClose("savedrive", NULL);

	m_currentSavingPlayerId = -1;
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

