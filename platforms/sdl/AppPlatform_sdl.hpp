#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "AppPlatform.hpp"

#ifdef ORIGINAL_CODE
#error "This isn't original code. You probably shouldn't try to compile this"
#endif

void ensure_screenshots_folder(const char *screenshots);

class AppPlatform_sdl : public AppPlatform
{
public:
	AppPlatform_sdl(std::string storageDir, SDL_Window *window);

#ifndef __EMSCRIPTEN__
	void saveScreenshot(const std::string& fileName, int width, int height) override;
#endif
	int checkLicense() override;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	Texture loadTexture(const std::string& str, bool b) override;
	int getUserInputStatus() override;

	// Also add these to allow proper turning within the game.
	void setMouseGrabbed(bool b) override;
	void setMouseDiff(int x, int y);
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);
private:
	std::string _storageDir;
	SDL_Window *_window;

	bool m_bShiftPressed[2] = {false, false};

	int xrel;
	int yrel;
};
