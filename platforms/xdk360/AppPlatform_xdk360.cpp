#include "AppPlatform_xdk360.hpp"

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "common/Utils.hpp"
#include "common/Util.hpp"
#include "renderer/RenderContextImmediate.hpp"

// requiring drive has at least 0 bytes free
#define C_SAVEDEVICE_MINIMUM_FREE_BYTES 0
#define C_SAVEDEVICE_ID_NONE 0
#define C_SAVEDEVICE_ID_PENDING -1
#define C_SAVEDEVICE_ID_ERROR -2

AppPlatform_xdk360::AppPlatform_xdk360()
{
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;

	m_bHasGraphics = false;
	m_bIsFocused = false;
	m_bWasUnfocused = false;

	m_pSoundSystem = nullptr;

	memset(m_saveDeviceId, C_SAVEDEVICE_ID_NONE, C_MAX_LOCAL_PLAYERS);
	m_currentSavingPlayerId = -1;

	m_bVirtualKeyboard = false;
	ZeroMemory(&m_vkOverlapped, sizeof(XOVERLAPPED));
}

AppPlatform_xdk360::~AppPlatform_xdk360()
{
	SAFE_DELETE(m_pSoundSystem);
}

void AppPlatform_xdk360::_getXContentData(XCONTENT_DATA& contentData, LocalPlayerID playerId)
{
	ZeroMemory(&contentData, sizeof(XCONTENT_DATA));
	lstrcpyW(contentData.szDisplayName, L"Profile Data");
	strcpy(contentData.szFileName, "profile.bin");
	contentData.dwContentType = XCONTENTTYPE_SAVEDGAME;
	contentData.DeviceID = _getSaveDeviceId(playerId);
}

const XCONTENTDEVICEID& AppPlatform_xdk360::_getSaveDeviceId(LocalPlayerID playerId)
{
	XCONTENTDEVICEID& deviceId = m_saveDeviceId[playerId];

	if (deviceId != C_SAVEDEVICE_ID_NONE)
		return deviceId;

	ULARGE_INTEGER iBytesRequested = { C_SAVEDEVICE_MINIMUM_FREE_BYTES };
	XOVERLAPPED xov = {0};

	deviceId = C_SAVEDEVICE_ID_PENDING;
	DWORD result = XShowDeviceSelectorUI(
		playerId,
		XCONTENTTYPE_SAVEDGAME, XCONTENTFLAG_NONE,
		iBytesRequested,
		&deviceId,
		&xov
	);

	if (result != ERROR_IO_PENDING)
	{
		LOG_W("Failed to open save device for player %d", playerId);
		deviceId = C_SAVEDEVICE_ID_ERROR;
	}

	// Wait for the save-device handle
	while (XHasOverlappedIoCompleted(&xov) == FALSE)
	{
		sleepMs(20);
	}

	return deviceId;
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
	return true;
}

std::string AppPlatform_xdk360::getAssetPath(const std::string& path) const
{
	std::string assetPath = AppPlatform::getAssetPath(path);
	makeNativePath(assetPath);
	return "GAME:\\" + assetPath;
}

void AppPlatform_xdk360::makeNativePath(std::string& path) const
{
	toDosPath((char*)path.c_str());
}

void AppPlatform_xdk360::beginProfileDataRead(LocalPlayerID playerId)
{
	if (m_currentSavingPlayerId != -1)
	{
		LOG_E("Tried to begin profile data read for player %d, but player %d is already saving data!", playerId, m_currentSavingPlayerId);
		throw std::bad_cast();
	}

	XCONTENT_DATA contentData;
	_getXContentData(contentData, playerId);

	XUID xuid;
	BOOL playerIsCreator;
	XContentGetCreator(playerId, &contentData, &playerIsCreator, &xuid, NULL);

	if(playerIsCreator == FALSE)
	{
		LOG_E("Current player is not the creator of, and therefore cannot write to, the requested profile data!");
		throw std::bad_cast();
	}

	// Mount the device to the "savedrive" drive name
	DWORD result = XContentCreate(playerId, "savedrive", &contentData, XCONTENTFLAG_OPENEXISTING, NULL, NULL, NULL);

	switch (result)
	{
	case ERROR_SUCCESS:
		m_currentSavingPlayerId = playerId;
		break;
	case ERROR_ACCESS_DENIED:
		LOG_E("beginProfileDataRead failed: XContentCreate returned access denied error!");
		break;
	case ERROR_INVALID_NAME:
		LOG_E("beginProfileDataRead failed: XContentCreate szRootName is invalid!");
		throw std::bad_cast();
	default:
		LOG_E("beginProfileDataRead failed: XContentCreate returned unknown error: %d", result);
	}
}

void AppPlatform_xdk360::endProfileDataRead(LocalPlayerID playerId)
{
	if (m_currentSavingPlayerId == -1)
	{
		LOG_E("Tried to end profile data read for player %d, but no one is saving any data!", playerId);
		throw std::bad_cast();
	}

	XContentClose("savedrive", NULL);

	m_currentSavingPlayerId = -1;
}

void AppPlatform_xdk360::beginProfileDataWrite(LocalPlayerID playerId)
{
	if (m_currentSavingPlayerId != -1)
	{
		LOG_E("Tried to begin profile data write for player %d, but player %d is already saving data!", playerId, m_currentSavingPlayerId);
		throw std::bad_cast();
	}

	XCONTENT_DATA contentData;
	_getXContentData(contentData, playerId);

	// Mount the device to the "savedrive" drive name
	DWORD result = XContentCreate(playerId, "savedrive", &contentData, XCONTENTFLAG_CREATEALWAYS, NULL, NULL, NULL);

	switch (result)
	{
	case ERROR_SUCCESS:
		m_currentSavingPlayerId = playerId;
		break;
	case ERROR_ACCESS_DENIED:
		LOG_E("beginProfileDataWrite failed: XContentCreate returned access denied error!");
		break;
	case ERROR_INVALID_NAME:
		LOG_E("beginProfileDataWrite failed: XContentCreate szRootName is invalid!");
		throw std::bad_cast();
	default:
		LOG_E("beginProfileDataWrite failed: XContentCreate returned unknown error: %d", result);
	}
}

void AppPlatform_xdk360::endProfileDataWrite(LocalPlayerID playerId)
{
	if (m_currentSavingPlayerId == -1)
	{
		LOG_E("Tried to end profile data write for player %d, but no one is saving any data!", playerId);
		throw std::bad_cast();
	}

	XContentClose("savedrive", NULL);

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

DWORD _getNativeVirtualKeyboardType(VirtualKeyboard::Type type)
{
	switch (type)
	{
	case VirtualKeyboard::EMAIL:             return VKBD_LATIN_EMAIL;
	case VirtualKeyboard::PLATFORM_USERNAME: return VKBD_LATIN_GAMERTAG;
	case VirtualKeyboard::PHONE_NUMBER:      return VKBD_LATIN_PHONE;
	case VirtualKeyboard::IP_ADDRESS:        return VKBD_LATIN_IP_ADDRESS;
	case VirtualKeyboard::NUMERIC:           return VKBD_LATIN_NUMERIC;
	case VirtualKeyboard::ALPHABET:          return VKBD_LATIN_ALPHABET;
	case VirtualKeyboard::PASSWORD:          return VKBD_LATIN_PASSWORD;
	default:                                 return VKBD_LATIN_FULL; // we don't support non-English in TextBoxes
	}

}

void AppPlatform_xdk360::showKeyboard(LocalPlayerID playerId, const VirtualKeyboard& keyboard)
{
	if (m_bVirtualKeyboard)
		return;

	ZeroMemory(&m_vkOverlapped, sizeof(XOVERLAPPED));
	m_vkResultText.clear(); // we can probably remove this later
	m_vkResultText.reserve(keyboard.maxLength > 0 ? keyboard.maxLength : 255);
	DWORD dwFlags = _getNativeVirtualKeyboardType(keyboard.type);
	m_vkDefaultText = Util::toWideString(keyboard.defaultText);
	m_vkTitleText = Util::toWideString(keyboard.titleText);
	m_vkDescriptionText = Util::toWideString(keyboard.descriptionText);

	DWORD result = XShowKeyboardUI(
         playerId,
         dwFlags,
         m_vkDefaultText.c_str(),
         m_vkTitleText.c_str(),
         m_vkDescriptionText.c_str(),
         (wchar_t*)m_vkResultText.data(),
         m_vkResultText.capacity(),
         &m_vkOverlapped
	);

	if (result != ERROR_IO_PENDING)
	{
		LOG_W("Failed to open virtual keyboard for player %d", playerId);
		return;
	}

	m_bVirtualKeyboard = true;
}

void AppPlatform_xdk360::hideKeyboard(LocalPlayerID playerId)
{
	// there's nothing we can really do here
}

void AppPlatform_xdk360::onHideKeyboard()
{
	m_bVirtualKeyboard = false;
	m_keyboardText = Util::toString(m_vkResultText.c_str());
	m_vkResultText.clear();
}

const std::string& AppPlatform_xdk360::getKeyboardText() const
{
	return m_keyboardText;
}

void AppPlatform_xdk360::setVSyncEnabled(bool enabled)
{
	// You have to reset/recreate the D3D device to change the vsync setting
	// @TODO: Someone with a windows machine or xbox please do and test this.
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

