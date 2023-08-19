#include "AppPlatform_sdlbase.hpp"

#include <sstream>
#include <fstream>
#include <sys/stat.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "compat/GL.hpp"
#endif

#include "common/Utils.hpp"

void AppPlatform_sdlbase::_init(std::string storageDir, SDL_Window *window)
{
    _storageDir = storageDir;
    _window = window;
    
    _iconTexture = nullptr;
    _icon = nullptr;
    
    m_bShiftPressed[0] = false;
    m_bShiftPressed[1] = false;

    ensureDirectoryExists(_storageDir.c_str());
}

void AppPlatform_sdlbase::_init(std::string storageDir, SDL_Window *window, const Texture& icon)
{
    _init(storageDir, window);
    
    _iconTexture = new Texture(icon);
    _icon = getSurfaceForTexture(_iconTexture);
    if (_icon)
        SDL_SetWindowIcon(window, _icon);
}

AppPlatform_sdlbase::~AppPlatform_sdlbase()
{
    SDL_FreeSurface(_icon);
    delete _iconTexture;
}

SDL_Surface* AppPlatform_sdlbase::getSurfaceForTexture(const Texture* const texture)
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
        LogMsg("Error loading SDL_Surface from Texture: %s", SDL_GetError());
    
    return surface;
}

int AppPlatform_sdlbase::checkLicense()
{
	// we own the game!!
	return 1;
}

const char* const AppPlatform_sdlbase::getWindowTitle() const
{
    return SDL_GetWindowTitle(_window);
}

int AppPlatform_sdlbase::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(_window, &width, nullptr);
	return width;
}

int AppPlatform_sdlbase::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(_window, nullptr, &height);
	return height;
}

void AppPlatform_sdlbase::setMouseGrabbed(bool b)
{
	SDL_SetWindowGrab(_window, b ? SDL_TRUE : SDL_FALSE);
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void AppPlatform_sdlbase::setMouseDiff(int x, int y)
{
	xrel = x;
	yrel = y;
}

void AppPlatform_sdlbase::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_sdlbase::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_sdlbase::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdlbase::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdlbase::getUserInputStatus()
{
	return -1;
}

Mouse::ButtonType AppPlatform_sdlbase::GetMouseButtonType(SDL_Event event)
{
	switch (event.button.button)
	{
	case SDL_BUTTON_LEFT:
		return Mouse::ButtonType::LEFT;
	case SDL_BUTTON_RIGHT:
		return Mouse::ButtonType::RIGHT;
	case SDL_BUTTON_MIDDLE:
		return Mouse::ButtonType::MIDDLE;
	default:
		return Mouse::ButtonType::NONE;
	}
}

Mouse::ButtonState AppPlatform_sdlbase::GetMouseButtonState(SDL_Event event)
{
	Mouse::ButtonState result;

	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		result = Mouse::ButtonState::DOWN;
		break;
	case SDL_MOUSEBUTTONUP:
		result = Mouse::ButtonState::UP;
		break;
	case SDL_MOUSEWHEEL:
	{
		short wheelDelta = event.wheel.y;
		if (wheelDelta > 0)
		{
			// "A positive value indicates that the wheel was rotated forward, away from the user."
			result = Mouse::ButtonState::UP;
		}
		else
		{
			// "A negative value indicates that the wheel was rotated backward, toward the user."
			result = Mouse::ButtonState::DOWN;
		}
		break;
	}
	default:
		result = Mouse::ButtonState::UP;
		break;
	}

	return result;
}

Keyboard::KeyState AppPlatform_sdlbase::GetKeyState(SDL_Event event)
{
	switch (event.key.state)
	{
	case SDL_RELEASED:
		return Keyboard::KeyState::UP;
	case SDL_PRESSED:
	default:
		return Keyboard::KeyState::DOWN;
	}
}
