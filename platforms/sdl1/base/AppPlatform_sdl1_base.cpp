#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

#include "AppPlatform_sdl1_base.hpp"

#include "thirdparty/GL/GL.hpp"

#include "common/Utils.hpp"
#include "CustomSoundSystem.hpp"
#include "client/player/input/Controller.hpp"

// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

void AppPlatform_sdl1_base::_init(std::string storageDir, SDL_Surface* screen)
{
    _storageDir = storageDir;
    _screen = screen;

    _iconTexture = nullptr;
    _icon = nullptr;

    m_bShiftPressed[0] = false;
    m_bShiftPressed[1] = false;

    ensureDirectoryExists(_storageDir.c_str());

    m_pSoundSystem = nullptr;
    m_bIsTouchscreen = false;

    const char* mode = getenv("MCPE_INPUT_MODE");
    _controller = findGameController();

    clearDiff();
}

void AppPlatform_sdl1_base::initSoundSystem()
{
    if (!m_pSoundSystem) {
        LOG_I("Initializing " STR(SOUND_SYSTEM) "...");
        m_pSoundSystem = new SOUND_SYSTEM();
    } else {
        LOG_E("Trying to initialize SoundSystem more than once!");
    }
}

void AppPlatform_sdl1_base::setIcon(const Texture& icon)
{
    if (!icon.m_pixels) return;

    SAFE_DELETE(_iconTexture);
    if (_icon) SDL_FreeSurface(_icon);

    _iconTexture = new Texture(icon);
    _icon = getSurfaceForTexture(_iconTexture);

    if (_icon) SDL_WM_SetIcon(_icon, nullptr);
}

AppPlatform_sdl1_base::~AppPlatform_sdl1_base()
{
    if (_icon) SDL_FreeSurface(_icon);
    SAFE_DELETE(_iconTexture);
    SAFE_DELETE(m_pSoundSystem);
}

SDL_Joystick* AppPlatform_sdl1_base::findGameController()
{
    if (SDL_NumJoysticks() > 0)
        return SDL_JoystickOpen(0);
    return nullptr;
}

SDL_Surface* AppPlatform_sdl1_base::getSurfaceForTexture(const Texture* const texture)
{
    if (!texture) return nullptr;

    void* pixels = texture->m_pixels;
    int width = texture->m_width;
    int height = texture->m_height;
    int depth = 32;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, depth,
        width * 4,
        0x000000FF, 0x0000FF00, 0x00FF0000,
        0xFF000000
    );

    if (!surface)
        LOG_E("Failed loading SDL_Surface from Texture: %s", SDL_GetError());

    return surface;
}

int AppPlatform_sdl1_base::checkLicense()
{
    return 1; // we own the game!!
}

const char* const AppPlatform_sdl1_base::getWindowTitle() const
{
    return "ReMinecraftPE";
}

int AppPlatform_sdl1_base::getScreenWidth() const
{
    return _screen ? _screen->w : 0;
}

int AppPlatform_sdl1_base::getScreenHeight() const
{
    return _screen ? _screen->h : 0;
}

void AppPlatform_sdl1_base::setMouseGrabbed(bool b)
{
    SDL_WM_GrabInput(b ? SDL_GRAB_ON : SDL_GRAB_OFF);
    SDL_ShowCursor(b ? SDL_FALSE : SDL_TRUE);
    clearDiff();
}

void AppPlatform_sdl1_base::setMouseDiff(int x, int y)
{
    xrel += x;
    yrel += y;
}

void AppPlatform_sdl1_base::getMouseDiff(int& x, int& y)
{
    x = xrel;
    y = yrel;
}

void AppPlatform_sdl1_base::clearDiff()
{
    xrel = 0;
    yrel = 0;
}

bool AppPlatform_sdl1_base::shiftPressed()
{
    return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl1_base::setShiftPressed(bool b, bool isLeft)
{
    m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl1_base::getUserInputStatus()
{
    return -1;
}

MouseButtonType AppPlatform_sdl1_base::GetMouseButtonType(Uint8 button)
{
    switch (button) {
        case SDL_BUTTON_LEFT: return BUTTON_LEFT;
        case SDL_BUTTON_RIGHT: return BUTTON_RIGHT;
        case SDL_BUTTON_MIDDLE: return BUTTON_MIDDLE;
        default: return BUTTON_NONE;
    }
}

bool AppPlatform_sdl1_base::GetMouseButtonState(const SDL_Event& event)
{
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: return true;
        case SDL_MOUSEBUTTONUP: return false;
        default: return false;
    }
}

Keyboard::KeyState AppPlatform_sdl1_base::GetKeyState(uint8_t state)
{
    return (state == SDL_RELEASED) ? Keyboard::UP : Keyboard::DOWN;
}

void AppPlatform_sdl1_base::showKeyboard(int, int, int, int)
{
}

void AppPlatform_sdl1_base::hideKeyboard()
{
}

bool AppPlatform_sdl1_base::isTouchscreen() const
{
    return false;
}

bool AppPlatform_sdl1_base::hasGamepad() const
{
    return _controller != nullptr;
}

void AppPlatform_sdl1_base::gameControllerAdded(int index)
{
    if (!_controller)
        _controller = SDL_JoystickOpen(index);
}

void AppPlatform_sdl1_base::gameControllerRemoved(int index)
{
    if (_controller && SDL_JoystickIndex(_controller) == index) {
        SDL_JoystickClose(_controller);
        _controller = findGameController();
    }
}

void AppPlatform_sdl1_base::handleKeyEvent(const SDL_Event& event)
{
    int key = event.key.keysym.sym;
    uint8_t state = event.key.state;

    switch (key) {
        case SDLK_F2:
            if (state == SDL_PRESSED)
                saveScreenshot("", -1, -1);
            return;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            setShiftPressed(state == SDL_PRESSED, key == SDLK_LSHIFT);
            break;
    }

    Keyboard::feed(GetKeyState(state), TranslateSDLKeyCodeToVirtual(key));
}

void AppPlatform_sdl1_base::handleButtonEvent(const SDL_Event& event)
{
    if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
        Keyboard::feed(GetKeyState(event.jbutton.state), event.jbutton.button);
}

void AppPlatform_sdl1_base::handleControllerAxisEvent(const SDL_Event& event)
{
    if (event.type != SDL_JOYAXISMOTION) return;

    float val = event.jaxis.value / 32767.0f;

    switch (event.jaxis.axis) {
        case 0: Controller::feedStickX(1, true, val); break;
        case 1: Controller::feedStickY(1, true, val); break;
        case 2: Controller::feedStickX(2, true, val); break;
        case 3: Controller::feedStickY(2, true, val); break;
    }
}

AssetFile AppPlatform_sdl1_base::readAssetFile(const std::string& str, bool quiet) const
{
    std::string path = getAssetPath(str);
    SDL_RWops* io = SDL_RWFromFile(path.c_str(), "rb");
    if (!io) {
        if (!quiet) LOG_W("Couldn't find asset file: %s", path.c_str());
        return AssetFile();
    }

    // SDL 1.2-compatible file size calculation
    Sint64 size = SDL_RWseek(io, 0, SEEK_END);
    SDL_RWseek(io, 0, SEEK_SET);
    if (size < 0) {
        if (!quiet) LOG_E("Error determining the size of the asset file!");
        SDL_RWclose(io);
        return AssetFile();
    }

    unsigned char* buf = new unsigned char[size];
    SDL_RWread(io, buf, size, 1);
    SDL_RWclose(io);

    return AssetFile(size, buf);
}

int AppPlatform_sdl1_base::TranslateSDLKeyCodeToVirtual(int sdlCode)
{
	switch (sdlCode) {
#define CODE(x) case SDLK_ ## x: return SDLVK_ ## x;
#include "compat/SDLKeyCodes.h"
#undef  CODE
	}
	return SDLVK_UNKNOWN;
}
