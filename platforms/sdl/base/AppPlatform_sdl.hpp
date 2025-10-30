#pragma once

#include <string>
#include "thirdparty/SDL/SDL.h"
#include "thirdparty/SDL/SDL_gamecontroller.h"

#include "client/app/AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "common/Logger.hpp"

class AppPlatform_sdl : public AppPlatform
{
private:
	void _init(std::string storageDir);

public:
	AppPlatform_sdl(std::string storageDir) { _init(storageDir); }
	~AppPlatform_sdl() override;

protected:
	virtual void _updateWindowIcon() {}
	virtual void _setMouseGrabbed(bool b) {}
	virtual void _handleKeyEvent(int key, uint8_t state);
	virtual void _ensureDirectoryExists(const char* path); // desktop only
	virtual void* _getHWND() const { return nullptr; }

	void _setIcon(const Texture& icon); // note: this takes ownership of the texture, so no memory leaks!
	void _setDefaultIcon();

public:
	virtual const char* getWindowTitle() const = 0;

	void initSoundSystem() override;

	int checkLicense() override;
	int getUserInputStatus() override;
	void saveScreenshot(const std::string& fileName, int width, int height) override;
	Texture loadTexture(const std::string& path, bool bIsRequired = false) override;
	bool doesTextureExist(const std::string& path) const override;
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }

	// Also add these to allow proper turning within the game.
	void setMouseGrabbed(bool b) override;
	void setMouseDiff(int x, int y);
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);

	// Configure Touchscreen
	bool isTouchscreen() const override;

	// Input events
	virtual void handleKeyEvent(const SDL_Event& event);
	void handleControllerButtonEvent(SDL_JoystickID controllerIndex, uint8_t button, uint8_t state);
	void handleControllerAxisEvent(SDL_JoystickID controllerIndex, uint8_t axis, int16_t value);

	// Needed for Android
	AssetFile readAssetFile(const std::string&, bool) const override;

protected:
	static SDL_Surface* _GetSurfaceForTexture(const Texture& texture);
	static int _SavePng(const char* filename, unsigned char* pixels, int line_size, int width, int height);
	static int _TranslateSDLKeyCodeToVirtual(int sdlCode);

public:
	static MouseButtonType GetMouseButtonType(uint8_t button);
	static bool GetMouseButtonState(const SDL_Event& event);
	static Keyboard::KeyState GetKeyState(uint8_t state);

protected:
	const Texture* m_pIconTexture;
	SDL_Surface* m_pIcon;
	bool m_bShiftPressed[2];

	int m_xrel;
	int m_yrel;

	SoundSystem* m_pSoundSystem;
	bool m_bIsTouchscreen;
	std::string m_storageDir;
};
