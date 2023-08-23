#pragma once

#include <string>
#include <SDL2/SDL.h>

#include "AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"

class AppPlatform_sdlbase : public AppPlatform
{
public:
    void _init(std::string storageDir, SDL_Window *window);
    AppPlatform_sdlbase(std::string storageDir, SDL_Window *window)
    {
        _init(storageDir, window);
    }
    ~AppPlatform_sdlbase();

	int checkLicense() override;
	const char* const getWindowTitle() const;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	Texture loadTexture(const std::string& path, bool b = false) override = 0;
	int getUserInputStatus() override;

	// Also add these to allow proper turning within the game.
	void setMouseGrabbed(bool b) override;
	void setMouseDiff(int x, int y);
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);

	static Mouse::ButtonType GetMouseButtonType(SDL_Event event);
	static Mouse::ButtonState GetMouseButtonState(SDL_Event event);
	static Keyboard::KeyState GetKeyState(SDL_Event event);
private:
	SDL_Window *_window;
    
    const Texture *_iconTexture;
    SDL_Surface *_icon;

	bool m_bShiftPressed[2];

	int xrel;
	int yrel;
    
    static SDL_Surface* getSurfaceForTexture(const Texture* const texture);
protected:
    std::string _storageDir;

	virtual void ensureDirectoryExists(const char* path) { }
	
	void setIcon(const Texture& icon); // note: this takes ownership of the texture, so no memory leaks!
};
