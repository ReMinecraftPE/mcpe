#pragma once

#include <string>
#include "thirdparty/SDL/SDL.h"

#include "../base/AppPlatform_sdl1.hpp"

class AppPlatform_sdl1_desktop : public AppPlatform_sdl1
{
public:
    AppPlatform_sdl1_desktop(std::string storageDir, SDL_Surface *screen);

    bool hasFileSystemAccess() override;

    void recenterMouse() override;
    bool getRecenterMouseEveryTick() override;
};
