#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>
#include <ctime>

#include "stb_image.h"
#include "AppPlatform_sdl.hpp"
#include "common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Surface* screen)
    : AppPlatform_sdl_base(storageDir, screen), m_screen(screen)
{
    /* ICON */
    SDL_Surface* icon = loadSurface("icon.bmp");
    if (icon) {
        SDL_WM_SetIcon(icon, nullptr);
        SDL_FreeSurface(icon);
    }
}


void AppPlatform_sdl::ensureDirectoryExists(const char* path)
{
    struct stat obj;
    if (stat(path, &obj) != 0 || !S_ISDIR(obj.st_mode)) {
#if defined(_WIN32) && !defined(__MINGW32__)
        int ret = XPL_MKDIR(path);
#else
        int ret = XPL_MKDIR(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        if (ret != 0) {
            LOG_E("Error Creating Directory: %s: %s", path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

/* Save */
void AppPlatform_sdl::saveScreenshot(const std::string& filename, int width, int height)
{
    std::string screenshots = _storageDir + "/screenshots";

    /* Filename with timecode */
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timeStr[256];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d_%H.%M.%S", timeinfo);

    ensureDirectoryExists(screenshots.c_str());

    std::string path = screenshots + "/";
    std::string file = path + timeStr + ".bmp";
    int num = 1;
    while (XPL_ACCESS(file.c_str(), F_OK) != -1) {
        file = path + SSTR(timeStr << "-" << num << ".bmp");
        num++;
    }

    /* Save */
    if (SDL_SaveBMP(m_screen, file.c_str()) != 0) {
        LOG_E("Screenshot Failed: %s", file.c_str());
    } else {
        LOG_I("Screenshot Saved: %s", file.c_str());
    }
}

/* Load bitmap */
SDL_Surface* AppPlatform_sdl::loadSurface(const std::string& path)
{
    std::string realPath = getAssetPath(path);
    SDL_Surface* surface = SDL_LoadBMP(realPath.c_str());
    if (!surface) {
        LOG_E("Couldn't load BMP: %s", path.c_str());
    }
    return surface;
}

/* Does bitmap exist */
bool AppPlatform_sdl::doesSurfaceExist(const std::string& path) const
{
    std::string realPath = getAssetPath(path);
    std::ifstream f(realPath.c_str());
    return f.good();
}

/* STUBBED */
Texture AppPlatform_sdl::loadTexture(const std::string& path, bool)
{
    LOG_W("loadTexture is not supported in SDL 1.2. Use loadSurface instead.");
    return Texture{};
}

/* STUBBED */
bool AppPlatform_sdl::doesTextureExist(const std::string&) const
{
    return false;
}

/* STUBBED */
/* Always access FS */
bool AppPlatform_sdl::hasFileSystemAccess()
{
    return true;
}

std::string AppPlatform_sdl::getPatchData()
{
    std::string path = getAssetPath("patches/patch_data.txt");
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        LOG_W("Couldn't find patch data file!");
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void AppPlatform_sdl::recenterMouse()
{
    SDL_WarpMouse(m_screen->w / 2, m_screen->h / 2);
}
