#include "AppPlatform_sdl_base.hpp"

#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "thirdparty/GL/GL.hpp"
#endif

#include "common/Utils.hpp"

#include "SoundSystemAL.hpp"

void AppPlatform_sdl_base::_init(std::string storageDir, SDL_Window *window)
{
	_storageDir = storageDir;
	_window = window;

	_iconTexture = nullptr;
	_icon = nullptr;

	m_bShiftPressed[0] = false;
	m_bShiftPressed[1] = false;

	_mousegrabbed = false;

	ensureDirectoryExists(_storageDir.c_str());

	m_pLogger = new Logger;
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
}

void AppPlatform_sdl_base::initSoundSystem()
{
	if (!m_pSoundSystem)
	{
		LOG_I("Initializing OpenAL SoundSystem...");
		m_pSoundSystem = new SoundSystemAL();
	}
	else
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
	}
}

std::string AppPlatform_sdl_base::getDateString(int time)
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

void AppPlatform_sdl_base::setIcon(const Texture& icon)
{
	if (!icon.m_pixels)
		return;

	SAFE_DELETE(_iconTexture);
	if (_icon) SDL_FreeSurface(_icon);

	_iconTexture = new Texture(icon);
	_icon = getSurfaceForTexture(_iconTexture);

	if (_icon)
		SDL_SetWindowIcon(_window, _icon);
}

AppPlatform_sdl_base::~AppPlatform_sdl_base()
{
	if (_icon) SDL_FreeSurface(_icon);
	SAFE_DELETE(_iconTexture);

	SAFE_DELETE(m_pSoundSystem);

	// DELETE THIS LAST
	SAFE_DELETE(m_pLogger);
}

SDL_Surface* AppPlatform_sdl_base::getSurfaceForTexture(const Texture* const texture)
{
	if (!texture) return nullptr;

	void * const pixels = texture->m_pixels;
	const int width = texture->m_width;
	const int height = texture->m_height;
	const int depth = 32; // Color depth (32-bit by default)
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
			pixels, width, height, depth,
			width * 4, // Pitch
			0x000000FF, 0x0000FF00, 0x00FF0000,
			0xFF000000
	);
	if (!surface)
		LOG_E("Failed loading SDL_Surface from Texture: %s", SDL_GetError());

	return surface;
}

int AppPlatform_sdl_base::checkLicense()
{
	// we own the game!!
	return 1;
}

const char* const AppPlatform_sdl_base::getWindowTitle() const
{
	return SDL_GetWindowTitle(_window);
}

int AppPlatform_sdl_base::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(_window, &width, nullptr);
	return width;
}

int AppPlatform_sdl_base::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(_window, nullptr, &height);
	return height;
}

void AppPlatform_sdl_base::setMouseGrabbed(bool b)
{
	_mousegrabbed = b;
	SDL_SetWindowGrab(_window, b ? SDL_TRUE : SDL_FALSE);
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void AppPlatform_sdl_base::setMouseDiff(int x, int y)
{
	xrel = x;
	yrel = y;

	// Keep the mouse centered if its grabbed
	if (_mousegrabbed)
	{
		int w = 0, h = 0;
		SDL_GetWindowSize(_window,&w,&h);
		SDL_WarpMouseInWindow(_window,w/2,h/2);
		Mouse::feed(MouseButtonType::BUTTON_NONE, false, w/2,h/2);
	}
}

void AppPlatform_sdl_base::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_sdl_base::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_sdl_base::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl_base::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl_base::getUserInputStatus()
{
	return -1;
}

MouseButtonType AppPlatform_sdl_base::GetMouseButtonType(SDL_Event event)
{
	switch (event.button.button)
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

bool AppPlatform_sdl_base::GetMouseButtonState(SDL_Event event)
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
		case SDL_MOUSEWHEEL:
		{
			short wheelDelta = event.wheel.y;
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

Keyboard::KeyState AppPlatform_sdl_base::GetKeyState(SDL_Event event)
{
	switch (event.key.state)
	{
		case SDL_RELEASED:
			return Keyboard::UP;
		case SDL_PRESSED:
		default:
			return Keyboard::DOWN;
	}
}

void AppPlatform_sdl_base::showKeyboard(int x, int y, int w, int h)
{
	if (SDL_IsTextInputActive())
	{
		hideKeyboard();
	}
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_SetTextInputRect(&rect);
	SDL_StartTextInput();
}

void AppPlatform_sdl_base::hideKeyboard()
{
	if (SDL_IsTextInputActive())
	{
		SDL_StopTextInput();
	}
}

bool AppPlatform_sdl_base::isTouchscreen() {
    return m_bIsTouchscreen;
}
