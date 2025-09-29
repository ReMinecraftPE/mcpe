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
	
	bool hasFileSystemAccess() override;

	void recenterMouse() override;
};
