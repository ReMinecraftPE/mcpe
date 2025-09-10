#pragma once

#include <string>
#include <SDL/SDL.h>

#include "../base/AppPlatform_sdl_base.hpp"

#define EM_BOOL bool
#define EM_TRUE true
#define EM_FALSE false

class AppPlatform_sdl : public AppPlatform_sdl_base
{
public:
    AppPlatform_sdl(std::string storageDir, SDL_Surface *screen);

    void saveScreenshot(const std::string& fileName, int width, int height) override;

    SDL_Surface* loadSurface(const std::string& path);      /* Use SDL Surface instead of Window (1.2 shit.) */
    bool doesSurfaceExist(const std::string& path) const;

    Texture loadTexture(const std::string& path, bool b = false) override;
    bool doesTextureExist(const std::string& path) const override;

    bool hasFileSystemAccess() override;
    std::string getPatchData() override;

    void recenterMouse() override;

protected:
    void ensureDirectoryExists(const char* path) override;

private:
    SDL_Surface* m_screen;  /* Surface */
};
