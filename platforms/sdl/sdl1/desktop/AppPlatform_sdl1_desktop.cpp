#include "AppPlatform_sdl1_desktop.hpp"

AppPlatform_sdl1_desktop::AppPlatform_sdl1_desktop(std::string storageDir, SDL_Surface* screen)
    : AppPlatform_sdl1(storageDir, screen)
{
	_setDefaultIcon();
}

bool AppPlatform_sdl1_desktop::hasFileSystemAccess()
{
    return true;
}

void AppPlatform_sdl1_desktop::recenterMouse()
{
    SDL_WarpMouse(m_pScreen->w / 2, m_pScreen->h / 2);
}

bool AppPlatform_sdl1_desktop::getRecenterMouseEveryTick()
{
    return false;
}
