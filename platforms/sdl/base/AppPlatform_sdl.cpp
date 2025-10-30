#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <errno.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "stb_image.h"
#include "stb_image_write.h"

#include "thirdparty/GL/GL.hpp"

#include "AppPlatform_sdl.hpp"

#include "common/Utils.hpp"
#include "compat/KeyCodes.hpp"

#include "CustomSoundSystem.hpp"
// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

#include "client/player/input/Controller.hpp"

void AppPlatform_sdl::_init(std::string storageDir)
{
	m_storageDir = storageDir;

	m_hWND = _getHWND();

	m_pIconTexture = nullptr;
	m_pIcon = nullptr;

	m_bShiftPressed[0] = false;
	m_bShiftPressed[1] = false;

	_ensureDirectoryExists(m_storageDir.c_str());

	m_pSoundSystem = nullptr;

	// Default Touchscreen Mode
#ifdef ANDROID
	m_bIsTouchscreen = true;
#else
	m_bIsTouchscreen = false;
#endif
	// Custom Touchscreen Mode
	const char *mode = getenv("MCPE_INPUT_MODE");
	if (mode)
	{
		if (strcmp(mode, "touch") == 0)
		{
			m_bIsTouchscreen = true;
		}
		else if (strcmp(mode, "mouse") == 0)
		{
			m_bIsTouchscreen = false;
		}
	}

	clearDiff();
}

AppPlatform_sdl::~AppPlatform_sdl()
{
	if (m_pIcon) SDL_FreeSurface(m_pIcon);
	SAFE_DELETE(m_pIconTexture);

	SAFE_DELETE(m_pSoundSystem);
}

void AppPlatform_sdl::_handleKeyEvent(int key, uint8_t state)
{
	// This really should be handled somewhere else.
	// Unforunately, there is no global keyboard handler.
	// Keyboard events are either handled in Screen::keyboardEvent
	// when a Screen is visible, or in Minecraft::tickInput
	// when LocalPlayer exists.
	switch (key)
	{
	case SDLVK_F2:
		if (state == SDL_PRESSED)
			saveScreenshot("", -1, -1);
		return;
	case SDLVK_BACKSPACE:
		// Text Editing (This is currently handled in handle_events() in platforms/sdl/main.cpp)
		/*if (state == SDL_PRESSED)
			g_pApp->handleCharInput('\b');*/
		break;
	case SDLVK_LSHIFT:
	case SDLVK_RSHIFT:
		setShiftPressed(state == SDL_PRESSED, key == SDLVK_LSHIFT);
		break;
	}

	// Normal Key Press
	Keyboard::feed(AppPlatform_sdl::GetKeyState(state), key);
}

// Ensure Screenshots Folder Exists
void AppPlatform_sdl::_ensureDirectoryExists(const char* path)
{
	// Check Screenshots Folder
	struct stat obj;
	if (stat(path, &obj) != 0 || !S_ISDIR(obj.st_mode))
	{
		// Create Screenshots Folder
#if defined(_WIN32) && !defined(__MINGW32__)
		int ret = XPL_MKDIR(path);
#else
		int ret = XPL_MKDIR(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		if (ret != 0)
		{
			// Unable To Create Folder
			LOG_E("Error Creating Directory: %s: %s", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void AppPlatform_sdl::_setIcon(const Texture& icon)
{
	if (!icon.m_pixels)
		return;

	SAFE_DELETE(m_pIconTexture);
	if (m_pIcon) SDL_FreeSurface(m_pIcon);

	m_pIconTexture = new Texture(icon);
	m_pIcon = _GetSurfaceForTexture(*m_pIconTexture);

	_updateWindowIcon();
}

void AppPlatform_sdl::_setDefaultIcon()
{
	_setIcon(loadTexture("icon.png", false));
}

void AppPlatform_sdl::initSoundSystem()
{
	if (m_pSoundSystem)
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
		return;
	}

	LOG_I("Initializing " STR(SOUND_SYSTEM) "...");
	m_pSoundSystem = new SOUND_SYSTEM();
}

int AppPlatform_sdl::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_sdl::setMouseGrabbed(bool b)
{
	_setMouseGrabbed(b);
	clearDiff();
}

void AppPlatform_sdl::setMouseDiff(int x, int y)
{
	m_xrel += x;
	m_yrel += y;
}

void AppPlatform_sdl::getMouseDiff(int& x, int& y)
{
	x = m_xrel;
	y = m_yrel;
}

void AppPlatform_sdl::clearDiff()
{
	m_xrel = 0;
	m_yrel = 0;
}

bool AppPlatform_sdl::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl::getUserInputStatus()
{
	return -1;
}

void AppPlatform_sdl::saveScreenshot(const std::string& filename, int glWidth, int glHeight)
{
	// Get Directory
	std::string screenshots = m_storageDir + "/screenshots";

	// Get Timestamp
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char time[256];
	strftime(time, sizeof(time), "%Y-%m-%d_%H.%M.%S", timeinfo);

	// Ensure Screenshots Folder Exists
	_ensureDirectoryExists(screenshots.c_str());

	// Prevent Overwriting Screenshots
	int num = 1;
	const std::string path = screenshots + "/";
	std::string file = path + time + ".png";
	while (XPL_ACCESS(file.c_str(), F_OK) != -1)
	{
		file = path + SSTR(time << "-" << num << ".png");
		num++;
	}

	// Get Image Size
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int x = viewport[0];
	int y = viewport[1];
	int width = viewport[2];
	int height = viewport[3];

	// Get Line Size
	int line_size = width * 4;
	{
		// Handle Alignment
		int alignment;
		glGetIntegerv(GL_PACK_ALIGNMENT, (GLint*)&alignment);
		// Round
		int diff = line_size % alignment;
		if (diff > 0)
		{
			line_size = line_size + (alignment - diff);
		}
	}
	int size = height * line_size;

	// Read Pixels
	bool fail = false;
	unsigned char* pixels = new unsigned char[size];
	if (!pixels)
	{
		fail = true;
	}
	else
	{
		glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	// Save Image
	if (fail || !_SavePng(file.c_str(), pixels, line_size, width, height))
	{
		LOG_E("Screenshot Failed: %s", file.c_str());
	}
	else
	{
		LOG_I("Screenshot Saved: %s", file.c_str());
	}

	// Free
	if (!pixels)
	{
		delete[] pixels;
	}
}

Texture AppPlatform_sdl::loadTexture(const std::string& path, bool bIsRequired)
{
	Texture out;
	out.m_hasAlpha = true;
	out.field_D = 0;

	// Get Full Path
	std::string realPath = getAssetPath(path);

	// Read File
	SDL_RWops* io = SDL_RWFromFile(realPath.c_str(), "rb");
	if (!io)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return out;
	}
	Sint64 size;
#if SDL_MAJOR_VERSION >= 2
	size = SDL_RWsize(io);
#else
	size = SDL_RWseek(io, 0, SEEK_END);
	SDL_RWseek(io, 0, SEEK_SET);
#endif
	unsigned char* file = new unsigned char[size];
	SDL_RWread(io, file, size, 1);
	SDL_RWclose(io);

	// Parse Image
	int width = 0, height = 0, channels = 0;
	stbi_uc* img = stbi_load_from_memory(file, size, &width, &height, &channels, STBI_rgb_alpha);
	delete[] file;
	if (!img)
	{
		// Failed To Parse Image
		LOG_E("The image could not be loaded properly: %s", path.c_str());
		return out;
	}

	// Copy Image
	uint32_t* img2 = new uint32_t[width * height];
	memcpy(img2, img, width * height * sizeof(uint32_t));
	stbi_image_free(img);

	// Create Texture
	out.m_width = width;
	out.m_height = height;
	out.m_pixels = img2;

	// Return
	return out;
}

bool AppPlatform_sdl::doesTextureExist(const std::string& path) const
{
	// Get Full Path
	std::string realPath = getAssetPath(path);

	// Open File
	SDL_RWops* io = SDL_RWFromFile(realPath.c_str(), "rb");
	if (!io)
	{
		// Does Not Exist
		return false;
	}
	else
	{
		// File Exists
		SDL_RWclose(io);
		return true;
	}
}

bool AppPlatform_sdl::isTouchscreen() const
{
    return m_bIsTouchscreen;
}

void AppPlatform_sdl::handleKeyEvent(const SDL_Event& event)
{
	int key = _TranslateSDLKeyCodeToVirtual(event.key.keysym.sym);
	uint8_t state = event.key.state;

	return _handleKeyEvent(key, state);
}

void AppPlatform_sdl::handleControllerButtonEvent(SDL_JoystickID controllerIndex, uint8_t button, uint8_t state)
{
	// Normal Key Press
	Keyboard::feed(GetKeyState(state), button);
}

void AppPlatform_sdl::handleControllerAxisEvent(SDL_JoystickID controllerIndex, uint8_t axis, int16_t value)
{
	float val = value / 32767.0f; // -32768 to 32767

	switch (axis)
	{
	case SDL_CONTROLLER_AXIS_LEFTX:
		Controller::feedStickX(1, true, val);
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		Controller::feedStickY(1, true, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		Controller::feedStickX(2, true, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		Controller::feedStickY(2, true, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		Controller::feedTrigger(1, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		Controller::feedTrigger(2, val);
		break;
	}
}

AssetFile AppPlatform_sdl::readAssetFile(const std::string& str, bool quiet) const
{
	std::string path = getAssetPath(str);
	SDL_RWops* io = SDL_RWFromFile(path.c_str(), "rb");

	// Open File
	if (!io)
	{
		if (!quiet) LOG_W("Couldn't find asset file: %s", path.c_str());
		return AssetFile();
	}

	// Get File Size
	Sint64 size;
#if SDL_MAJOR_VERSION >= 2
	size = SDL_RWsize(io);
#else
	size = SDL_RWseek(io, 0, SEEK_END);
	SDL_RWseek(io, 0, SEEK_SET);
#endif
	if (size < 0)
	{
		if (!quiet) LOG_E("Error determining the size of the asset file!");
		SDL_RWclose(io);
		return AssetFile();
	}

	// Read Data
	unsigned char* buf = new unsigned char[size];
	SDL_RWread(io, buf, size, 1);

	// Close File
	SDL_RWclose(io);

	// Return
	return AssetFile(size, buf);
}

SDL_Surface* AppPlatform_sdl::_GetSurfaceForTexture(const Texture& texture)
{
	void* pixels = texture.m_pixels;
	int width = texture.m_width;
	int height = texture.m_height;
	int depth = 32; // Color depth (32-bit by default)

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		pixels, width, height, depth,
		width * 4, // Pitch
#if MC_ENDIANNESS_LITTLE
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else // MC_ENDIANNESS_BIG
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
	);
	if (!surface)
		LOG_E("Failed loading SDL_Surface from Texture: %s", SDL_GetError());

	return surface;
}

// Take Screenshot
int AppPlatform_sdl::_SavePng(const char* filename, unsigned char* pixels, int line_size, int width, int height)
{
	// Setup
	stbi_flip_vertically_on_write(true);

	// Write Image
	return stbi_write_png(filename, width, height, 4, pixels, line_size);
}

int AppPlatform_sdl::_TranslateSDLKeyCodeToVirtual(int sdlCode)
{
	switch (sdlCode) {
#define CODE(x) case SDLK_ ## x: return SDLVK_ ## x;
#include "compat/SDLKeyCodes.h"
#undef  CODE
	}
	return SDLVK_UNKNOWN;
}

MouseButtonType AppPlatform_sdl::GetMouseButtonType(uint8_t button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT:
		return BUTTON_LEFT;
	case SDL_BUTTON_RIGHT:
		return BUTTON_RIGHT;
	case SDL_BUTTON_MIDDLE:
		return BUTTON_MIDDLE;
	default:
		return BUTTON_NONE;
	}
}

bool AppPlatform_sdl::GetMouseButtonState(const SDL_Event& event)
{
	bool result;

	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		result = true;
		break;
	case SDL_MOUSEBUTTONUP:
		result = false;
		break;
	default:
		result = false;
		break;
	}

	return result;
}

Keyboard::KeyState AppPlatform_sdl::GetKeyState(uint8_t state)
{
	switch (state)
	{
	case SDL_RELEASED:
		return Keyboard::UP;
	case SDL_PRESSED:
	default:
		return Keyboard::DOWN;
	}
}
