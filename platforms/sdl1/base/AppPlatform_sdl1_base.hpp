#pragma once

#include <string>
#include <SDL/SDL.h>  // SDL 1.2 header

#include "client/app/AppPlatform.hpp"
#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "common/Logger.hpp"

class AppPlatform_sdl1_base : public AppPlatform
{
public:
    void _init(std::string storageDir, SDL_Surface* screen);
    AppPlatform_sdl1_base(std::string storageDir, SDL_Surface* screen)
    {
        _init(storageDir, screen);
    }
    ~AppPlatform_sdl1_base() override;

    void initSoundSystem() override;

    int checkLicense() override;
    const char* const getWindowTitle() const;
    int getScreenWidth() const override;
    int getScreenHeight() const override;

    SDL_Surface* loadSurface(const std::string& path);
    bool doesSurfaceExist(const std::string& path) const;

    int getUserInputStatus() override;
    SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }

    void setMouseGrabbed(bool b) override;
    void setMouseDiff(int x, int y);
    void getMouseDiff(int& x, int& y) override;
    void clearDiff() override;

    bool shiftPressed() override;
    void setShiftPressed(bool b, bool isLeft);

    static MouseButtonType GetMouseButtonType(Uint8 button);
    static bool GetMouseButtonState(const SDL_Event& event);
    static Keyboard::KeyState GetKeyState(uint8_t state);
    static int TranslateSDLKeyCodeToVirtual(int sdlCode);

    bool isTouchscreen() const override;

    // Game controller
    bool hasGamepad() const override;
    SDL_Joystick* getPrimaryGameController() const { return _controller; }
    void setPrimaryGameController(SDL_Joystick* controller) { _controller = controller; }
    void gameControllerAdded(int index);
    void gameControllerRemoved(int index);

    void handleKeyEvent(const SDL_Event& event);
    void handleButtonEvent(const SDL_Event& event);
    void handleControllerAxisEvent(const SDL_Event& event);

    // Read Sounds
    AssetFile readAssetFile(const std::string&, bool) const override;

protected:
    SDL_Surface* _screen;

private:
    SDL_Joystick* _controller;

    const Texture* _iconTexture;
    SDL_Surface* _icon;

    bool m_bShiftPressed[2];

    int xrel;
    int yrel;

    SoundSystem* m_pSoundSystem;

    bool m_bIsTouchscreen;

    SDL_Joystick* findGameController();

    static SDL_Surface* getSurfaceForTexture(const Texture* const texture);

protected:
    std::string _storageDir;

    virtual void ensureDirectoryExists(const char* path) { }

    void setIcon(const Texture& icon);
};
