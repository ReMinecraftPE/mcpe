#pragma once

#include <string>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../base/AppPlatform_sdl2.hpp"

class AppPlatform_sdl2_emscripten : public AppPlatform_sdl2
{
public:
	AppPlatform_sdl2_emscripten(std::string storageDir, SDL_Window *window);

	Texture loadTexture(const std::string& path, bool b = false) override;
	bool doesTextureExist(const std::string& path) const override;
};
