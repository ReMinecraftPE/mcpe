#pragma once

#include <string>

#include "../base/AppPlatform_sdl2.hpp"

#define EM_BOOL bool
#define EM_TRUE true
#define EM_FALSE false

class AppPlatform_sdl2_desktop : public AppPlatform_sdl2
{
public:
	AppPlatform_sdl2_desktop(std::string storageDir, SDL_Window *window);

	void saveScreenshot(const std::string& fileName, int width, int height) override;
	Texture loadTexture(const std::string& path, bool b = false) override;
	bool doesTextureExist(const std::string& path) const override;
	
	bool hasFileSystemAccess() override;
    std::string getPatchData() override;

	void recenterMouse() override;
	
protected:
	void ensureDirectoryExists(const char* path) override;
};
