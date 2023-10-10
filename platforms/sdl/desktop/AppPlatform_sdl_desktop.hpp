#pragma once

#include <string>

#include "AppPlatform_sdl_base.hpp"

class AppPlatform_sdl_desktop : public AppPlatform_sdl_base
{
public:
	AppPlatform_sdl_desktop(std::string storageDir, SDL_Window *window);

	void saveScreenshot(const std::string& fileName, int width, int height) override;
	Texture loadTexture(const std::string& path, bool b = false) override;
	
	bool hasFileSystemAccess() override;
	
protected:
	void ensureDirectoryExists(const char* path) override;
};
