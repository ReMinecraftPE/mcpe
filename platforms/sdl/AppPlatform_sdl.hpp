#pragma once

#include <string>

#include "AppPlatform_sdlbase.hpp"

class AppPlatform_sdl : public AppPlatform_sdlbase
{
public:
	AppPlatform_sdl(std::string storageDir, SDL_Window *window);

	void saveScreenshot(const std::string& fileName, int width, int height) override;
	Texture loadTexture(const std::string& path, bool b = false) override;
protected:
	void ensureDirectoryExists(const char* path) override;
};
