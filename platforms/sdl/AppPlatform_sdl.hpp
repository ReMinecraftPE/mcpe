#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "AppPlatform_sdlbase.hpp"

void ensure_screenshots_folder(const char *screenshots);

class AppPlatform_sdl : public AppPlatform_sdlbase
{
public:
	AppPlatform_sdl(std::string storageDir, SDL_Window *window);

	void saveScreenshot(const std::string& fileName, int width, int height) override;
	Texture loadTexture(const std::string& path, bool b = false) override;
};
