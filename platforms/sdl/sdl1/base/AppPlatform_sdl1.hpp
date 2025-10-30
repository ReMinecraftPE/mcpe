#pragma once

#include <string>
#include "thirdparty/SDL/SDL.h"

#include "common/Logger.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"

#include "platforms/sdl/base/AppPlatform_sdl.hpp"

class AppPlatform_sdl1 : public AppPlatform_sdl
{
private:
    void _init(std::string storageDir, SDL_Surface* screen);

public:
    AppPlatform_sdl1(std::string storageDir, SDL_Surface* screen)
        : AppPlatform_sdl(storageDir)
    {
        _init(storageDir, screen);
    }
    ~AppPlatform_sdl1() override;

private:
    SDL_Joystick* _findJoystick();

protected:
    void _updateWindowIcon() override;
    void _setMouseGrabbed(bool b) override;
    void* _getHWND() const override;

public:
    const char* getWindowTitle() const override;

    int getScreenWidth() const override;
    int getScreenHeight() const override;

    // Game controller
    bool hasGamepad() const override;
    SDL_Joystick* getPrimaryJoystick() const { return m_pJoystick; }
    void setPrimaryJoystick(SDL_Joystick* joystick) { m_pJoystick = joystick; }
    void joystickAdded(int index);
    void joystickRemoved(int index);

private:
    SDL_Joystick* m_pJoystick;

protected:
    SDL_Surface* m_pScreen;
};
