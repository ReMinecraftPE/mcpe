#pragma once

#include <string>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../base/AppPlatform_sdl_base.hpp"

class AppPlatform_sdl : public AppPlatform_sdl_base
{
public:
	AppPlatform_sdl(std::string storageDir, SDL_Window *window);

	Texture loadTexture(const std::string& path, bool b = false) override;
	bool doesTextureExist(const std::string& path) override;
};
