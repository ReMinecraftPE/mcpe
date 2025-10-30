#include "AppPlatform_sdl1.hpp"

#include "thirdparty/SDL/SDL_gamecontroller.h"
#include "thirdparty/GL/GL.hpp"

#include "CustomSoundSystem.hpp"
#include "client/player/input/Controller.hpp"

// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

void AppPlatform_sdl1::_init(std::string storageDir, SDL_Surface* screen)
{
    m_pScreen = screen;

    // Look for a pre-existing joystick
    m_pJoystick = _findJoystick();
}

AppPlatform_sdl1::~AppPlatform_sdl1()
{
}

SDL_Joystick* AppPlatform_sdl1::_findJoystick()
{
    if (SDL_NumJoysticks() > 0)
        return SDL_JoystickOpen(0);
    return nullptr;
}

void AppPlatform_sdl1::_updateWindowIcon()
{
	if (m_pIcon)
        SDL_WM_SetIcon(m_pIcon, nullptr);
}

void AppPlatform_sdl1::_setMouseGrabbed(bool b)
{
    SDL_WM_GrabInput(b ? SDL_GRAB_ON : SDL_GRAB_OFF);
    SDL_ShowCursor(b ? SDL_FALSE : SDL_TRUE);
}

void* AppPlatform_sdl1::_getHWND() const
{
#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWMInfo(&wmInfo);
    return wmInfo.window;
#else
    return AppPlatform_sdl::_getHWND();
#endif
}

const char* AppPlatform_sdl1::getWindowTitle() const
{
    char* title;
    SDL_WM_GetCaption(&title, nullptr);
    return title;
}

int AppPlatform_sdl1::getScreenWidth() const
{
    return m_pScreen ? m_pScreen->w : 0;
}

int AppPlatform_sdl1::getScreenHeight() const
{
    return m_pScreen ? m_pScreen->h : 0;
}

bool AppPlatform_sdl1::hasGamepad() const
{
    return m_pJoystick != nullptr;
}

void AppPlatform_sdl1::joystickAdded(int index)
{
    if (!m_pJoystick)
        m_pJoystick = SDL_JoystickOpen(index);
}

void AppPlatform_sdl1::joystickRemoved(int index)
{
    if (m_pJoystick && SDL_JoystickIndex(m_pJoystick) == index)
    {
        SDL_JoystickClose(m_pJoystick);
        m_pJoystick = _findJoystick();
    }
}