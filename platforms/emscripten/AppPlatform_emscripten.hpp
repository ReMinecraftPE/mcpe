#pragma once

#include <string>

#include "AppPlatform_sdl.hpp"

class AppPlatform_emscripten : public AppPlatform_sdlbase
{
public:
	AppPlatform_emscripten(std::string storageDir, SDL_Window *window);

	Texture loadTexture(const std::string& path, bool b = false) override;
};
