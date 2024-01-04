#pragma once

#include <string>
#include "thirdparty/SDL2/SDL2.h"

#include "client/app/AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "common/Logger.hpp"

class AppPlatform_sdl_base : public AppPlatform
{
public:
    void _init(std::string storageDir, SDL_Window *window);
	AppPlatform_sdl_base(std::string storageDir, SDL_Window *window)
    {
        _init(storageDir, window);
    }
	~AppPlatform_sdl_base();

	void initSoundSystem() override;

	int checkLicense() override;
	const char* const getWindowTitle() const;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	Texture loadTexture(const std::string& path, bool bIsRequired = false) override = 0;
	virtual bool doesTextureExist(const std::string& path) = 0;
	int getUserInputStatus() override;
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }
	std::string getDateString(int time) override;
	
	// Also add these to allow proper turning within the game.
	void setMouseGrabbed(bool b) override;
	void setMouseDiff(int x, int y);
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);

	static MouseButtonType GetMouseButtonType(SDL_Event event);
	static bool GetMouseButtonState(SDL_Event event);
	static Keyboard::KeyState GetKeyState(SDL_Event event);

	// On-screen keyboard
	void showKeyboard(int x, int y, int w, int h) override;
	void hideKeyboard() override;

	// Configure Touchscreen
	bool isTouchscreen() override;
private:
	SDL_Window *_window;

	const Texture *_iconTexture;
	SDL_Surface *_icon;

	bool m_bShiftPressed[2];

	int xrel;
	int yrel;

	bool _mousegrabbed;

	Logger* m_pLogger;
	SoundSystem* m_pSoundSystem;

	bool m_bIsTouchscreen;

	static SDL_Surface* getSurfaceForTexture(const Texture* const texture);
protected:
	std::string _storageDir;

	virtual void ensureDirectoryExists(const char* path) { }

	void setIcon(const Texture& icon); // note: this takes ownership of the texture, so no memory leaks!
};
