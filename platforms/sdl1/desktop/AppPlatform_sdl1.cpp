#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>
#include <ctime>

#include "stb_image.h"
#include "AppPlatform_sdl1.hpp"
#include "common/Utils.hpp"

AppPlatform_sdl1::AppPlatform_sdl1(std::string storageDir, SDL_Surface* screen)
    : AppPlatform_sdl1_base(storageDir, screen), m_screen(screen)
{
    /* ICON */
    SDL_Surface* icon = loadSurface("icon.bmp");
    if (icon) {
        SDL_WM_SetIcon(icon, nullptr);
        SDL_FreeSurface(icon);
    }
}


void AppPlatform_sdl1::ensureDirectoryExists(const char* path)
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
void AppPlatform_sdl1::saveScreenshot(const std::string& filename, int width, int height)
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
SDL_Surface* AppPlatform_sdl1::loadSurface(const std::string& path)
{
    std::string realPath = getAssetPath(path);
    SDL_Surface* surface = SDL_LoadBMP(realPath.c_str());
    if (!surface) {
        LOG_E("Couldn't load BMP: %s", path.c_str());
    }
    return surface;
}

/* Does bitmap exist */
bool AppPlatform_sdl1::doesSurfaceExist(const std::string& path) const
{
    std::string realPath = getAssetPath(path);
    std::ifstream f(realPath.c_str());
    return f.good();
}

/* Load bitmap (texture) */
Texture AppPlatform_sdl1::loadTexture(const std::string& path, bool bIsRequired)
{
    Texture out;
    out.m_hasAlpha = true;
    out.field_D = 0;

    // Get Full Path
    std::string realPath = getAssetPath(path);

    // Read File
    SDL_RWops *io = SDL_RWFromFile(realPath.c_str(), "rb");
    if (!io)
    {
        LOG_E("Couldn't find file: %s", path.c_str());
        return out;
    }
    Sint64 size = SDL_RWseek(io, 0, RW_SEEK_END);
    SDL_RWseek(io, 0, RW_SEEK_SET);
    unsigned char *file = new unsigned char[size];
    SDL_RWread(io, file, size, 1);
    SDL_RWclose(io);

    // Parse Image
    int width = 0, height = 0, channels = 0;
    stbi_uc *img = stbi_load_from_memory(file, static_cast<int>(size), &width, &height, &channels, STBI_rgb_alpha);
    delete[] file;
    if (!img)
    {
        // Failed To Parse Image
        LOG_E("The image could not be loaded properly: %s", path.c_str());
        return out;
    }

    // Copy Image
    uint32_t *img2 = new uint32_t[width * height];
    memcpy(img2, img, width * height * sizeof (uint32_t));
    stbi_image_free(img);

    // Create Texture
    out.m_width = width;
    out.m_height = height;
    out.m_pixels = img2;

    // Return
    return out;
}

/* STUBBED */
bool AppPlatform_sdl1::doesTextureExist(const std::string&) const
{
    return false;
}

/* STUBBED */
/* Always access FS */
bool AppPlatform_sdl1::hasFileSystemAccess()
{
    return true; /* TODO: Match SDL 2.x code (Do I need to fix this, no idea; I just stubbed all the functions at first then started eyeballing shit. )*/
}

std::string AppPlatform_sdl1::getPatchData()
{
    // @TODO: Why are we overriding AppPlatform::getPatchData()? It uses the standard library.
    
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

void AppPlatform_sdl1::recenterMouse()
{
    SDL_WarpMouse(m_screen->w / 2, m_screen->h / 2);
}

bool AppPlatform_sdl1::getRecenterMouseEveryTick()
{
    return false;
}
