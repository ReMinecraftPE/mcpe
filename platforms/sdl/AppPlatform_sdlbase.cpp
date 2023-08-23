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

void AppPlatform_sdlbase::SetDefaultBindings(KeyMapping keymap[])
{
#define KM(idx,code) keymap[idx].value = code
	KM(KM_FORWARD, SDLVK_w);
	KM(KM_LEFT, SDLVK_a);
	KM(KM_BACKWARD, SDLVK_s);
	KM(KM_RIGHT, SDLVK_d);
	KM(KM_JUMP, SDLVK_SPACE);
	KM(KM_DESTROY, SDLVK_x);
	KM(KM_PLACE, SDLVK_c);
	KM(KM_MENU_NEXT, SDLVK_DOWN);
	KM(KM_MENU_PREVIOUS, SDLVK_UP);
	KM(KM_MENU_OK, SDLVK_RETURN);
	KM(KM_MENU_CANCEL, SDLVK_ESCAPE);
	KM(KM_DROP, SDLVK_q);
	KM(KM_CHAT, SDLVK_t);
	KM(KM_FOG, SDLVK_f);
	KM(KM_INVENTORY, SDLVK_e);
	KM(KM_SNEAK, SDLVK_LSHIFT);
	KM(KM_SLOT_1, SDLVK_1);
	KM(KM_SLOT_2, SDLVK_2);
	KM(KM_SLOT_3, SDLVK_3);
	KM(KM_SLOT_4, SDLVK_4);
	KM(KM_SLOT_5, SDLVK_5);
	KM(KM_SLOT_6, SDLVK_6);
	KM(KM_SLOT_7, SDLVK_7);
	KM(KM_SLOT_8, SDLVK_8);
	KM(KM_SLOT_9, SDLVK_9);
	KM(KM_TOGGLEGUI, SDLVK_F1);
	KM(KM_SCREENSHOT, SDLVK_F2);
	KM(KM_TOGGLEDEBUG, SDLVK_F3);
	KM(KM_TOGGLEAO, SDLVK_F4);
	KM(KM_TOGGLE3RD, SDLVK_F5);
	KM(KM_SLOT_L, SDLVK_y);
	KM(KM_SLOT_R, SDLVK_u);
	KM(KM_FLY_UP, SDLVK_c);
	KM(KM_FLY_DOWN, SDLVK_x);
	KM(KM_CHAT_CMD, SDLVK_SLASH);
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
