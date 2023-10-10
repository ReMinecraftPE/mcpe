#pragma once

#include <string>

#include "AppPlatform_sdl_base.hpp"

class AppPlatform_sdl : public AppPlatform_sdl_base
{
public:
	AppPlatform_sdl(std::string storageDir, SDL_Window *window);

	Texture loadTexture(const std::string& path, bool b = false) override;
};
