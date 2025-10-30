#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

#include "AppPlatform_sdl2.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "thirdparty/GL/GL.hpp"
#endif

#include "compat/KeyCodes.hpp"

#include "CustomSoundSystem.hpp"
// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

#include "client/player/input/Controller.hpp"

void AppPlatform_sdl2::_init(SDL_Window *window)
{
	m_pWindow = window;

	// Look for a pre-existing controller
	m_pController = _findGameController();
}

AppPlatform_sdl2::~AppPlatform_sdl2()
{
}

SDL_GameController* AppPlatform_sdl2::_findGameController()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

void AppPlatform_sdl2::_updateWindowIcon()
{
	if (m_pWindow && m_pIcon)
		SDL_SetWindowIcon(m_pWindow, m_pIcon);
}

void AppPlatform_sdl2::_setMouseGrabbed(bool b)
{
	SDL_SetWindowGrab(m_pWindow, b ? SDL_TRUE : SDL_FALSE);
	/**
	 * @NOTE: There is a bug with older versions of SDL2 (ex: 2.0.4) where disabling RelativeMouseMode will cause a mouse event to be fired
	 * that just moves the cursor to where it would've been if the mode had never been enabled in the first place, effectively uncentering it.
	 * https://github.com/libsdl-org/SDL/issues/6002 (I'm not sure if this is the right issue, I just updated SDL after seeing this and it fixed the above problem.)
	 **/
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void AppPlatform_sdl2::_handleKeyEvent(int key, uint8_t state)
{
	switch (key)
	{
	case SDLVK_AC_BACK:
		// Android Back Button (This is currently handled in handle_events() in platforms/sdl/main.cpp)
		// @TODO: handleBack function in AppPlatform that calls back to App via function pointer
		//g_pApp->handleBack(event.key.state == SDL_PRESSED);
		return;
	}

	return AppPlatform_sdl::_handleKeyEvent(key, state);
}

void* AppPlatform_sdl2::_getHWND() const
{
#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &wmInfo);
	return wmInfo.info.win.window;
#else
	return AppPlatform_sdl::_getHWND();
#endif
}

const char* AppPlatform_sdl2::getWindowTitle() const
{
	return SDL_GetWindowTitle(m_pWindow);
}

int AppPlatform_sdl2::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(m_pWindow, &width, nullptr);
	return width;
}

int AppPlatform_sdl2::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(m_pWindow, nullptr, &height);
	return height;
}

void AppPlatform_sdl2::showKeyboard(int x, int y, int w, int h)
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

void AppPlatform_sdl2::hideKeyboard()
{
	if (SDL_IsTextInputActive())
	{
		SDL_StopTextInput();
	}
}

bool AppPlatform_sdl2::hasGamepad() const
{
	return m_pController != nullptr;
}

void AppPlatform_sdl2::gameControllerAdded(int32_t index)
{
	if (!getPrimaryGameController())
	{
		setPrimaryGameController(SDL_GameControllerOpen(index));
		Controller::reset();
	}
}

void AppPlatform_sdl2::gameControllerRemoved(int32_t index)
{
	SDL_GameController* controller = getPrimaryGameController();
	SDL_JoystickID joystickId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
	// Check if current controller has been removed
	if (controller && index == joystickId)
	{
		SDL_GameControllerClose(controller);
		// Hunt for a new primary controller
		setPrimaryGameController(_findGameController());
	}
}

bool AppPlatform_sdl2::GetMouseButtonState(const SDL_Event& event)
{
	bool result;

	switch (event.type)
	{
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
		result = AppPlatform_sdl::GetMouseButtonState(event);
		break;
	}

	return result;
}