#include "AppPlatform_sdl2_desktop.hpp"

AppPlatform_sdl2_desktop::AppPlatform_sdl2_desktop(std::string storageDir, SDL_Window *window)
	: AppPlatform_sdl2(storageDir, window)
{
	_setDefaultIcon();
}

bool AppPlatform_sdl2_desktop::hasFileSystemAccess()
{
	return true;
}

void AppPlatform_sdl2_desktop::recenterMouse()
{
	// Note. The only reason we do it this way instead of
	// using the Mouse class is because, after SDL_WarpMouseInWindow,
	// we'll get an event on our window telling us "hey, the
	// user has moved their cursor back to the center! Move
	// the camera back as well", causing a camera that just
	// refuses to move
	int w = 0, h = 0;
	SDL_GetWindowSize(m_pWindow, &w, &h);
	SDL_WarpMouseInWindow(m_pWindow, w / 2, h / 2);
	//Mouse::feed(BUTTON_NONE, false, w / 2, h / 2);
}