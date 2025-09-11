#include <stdarg.h>
#include <SDL/SDL.h>
#include <GL/gl.h>

#include "client/app/App.hpp"
#include "desktop/AppPlatform_sdl.hpp"
typedef AppPlatform_sdl UsedAppPlatform;

#include "client/app/NinecraftApp.hpp"
#include "client/player/input/Multitouch.hpp"

static float g_fPointToPixelScale = 1.0f;

UsedAppPlatform* g_pAppPlatform;
NinecraftApp* g_pApp;

SDL_Surface* screen = NULL;

static void teardown()
{
    if (screen != NULL)
    {
        SDL_Quit();
        screen = NULL;
    }
}

// Handle Events
static bool window_resized = false;
static void handle_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                SDL_Event newEvent;
                newEvent.type = event.type;
                newEvent.key.keysym.sym = (SDLKey)((unsigned int)event.key.keysym.sym);
                newEvent.key.state = event.key.state;

                if (newEvent.key.keysym.sym == SDLK_BACKSPACE && newEvent.key.state == SDL_PRESSED)
                {
                    g_pApp->handleCharInput('\b');
                }

                if (newEvent.type == SDL_KEYDOWN && newEvent.key.keysym.unicode > 0)
                {
                    g_pApp->handleCharInput(newEvent.key.keysym.unicode);
                }

                g_pAppPlatform->handleKeyEvent(newEvent);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                const float scale = g_fPointToPixelScale;
                MouseButtonType type = AppPlatform_sdl_base::GetMouseButtonType(event.button.button);
                bool state = AppPlatform_sdl_base::GetMouseButtonState(event);
                float x = event.button.x * scale;
                float y = event.button.y * scale;
                Mouse::feed(type, state, x, y);
                break;
            }
            case SDL_MOUSEMOTION:
            {
                float scale = g_fPointToPixelScale;
                float x = event.motion.x * scale;
                float y = event.motion.y * scale;
                Mouse::feed(BUTTON_NONE, false, x, y);
                g_pAppPlatform->setMouseDiff(event.motion.xrel * scale, event.motion.yrel * scale);
                break;
            }
            case SDL_VIDEORESIZE:
            {
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, SDL_OPENGL | SDL_RESIZABLE);
                window_resized = true;
                break;
            }
            case SDL_QUIT:
            {
                g_pApp->quit();
                break;
            }
        }
    }
}

// Resizing
static void resize()
{
    Minecraft::width  = screen->w;
    Minecraft::height = screen->h;

    g_fPointToPixelScale = float(screen->w) / float(screen->w);

    Minecraft::setRenderScaleMultiplier(g_fPointToPixelScale);

    if (g_pApp)
        g_pApp->sizeUpdate(screen->w, screen->h);
}

// Main Loop
static void main_loop()
{
    handle_events();

    if (window_resized)
    {
        window_resized = false;
        resize();
    }

    g_pApp->update();

    SDL_GL_SwapBuffers();

    if (g_pApp->wantToQuit())
    {
        g_pApp->saveOptions();
        delete g_pApp;
        delete g_pAppPlatform;
        teardown();
        exit(0);
    }
}

// Main
int main(int argc, char* argv[])
{
    Logger::setSingleton(new Logger);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __EMSCRIPTEN__
    Minecraft::width = std::stoi(argv[1]);
    Minecraft::height = std::stoi(argv[2]);
#else
    Minecraft::width = 800;
    Minecraft::height = 600;
#endif

    screen = SDL_SetVideoMode(Minecraft::width, Minecraft::height, 0, SDL_OPENGL | SDL_RESIZABLE);
    if (!screen)
    {
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    xglInit();
    if (!xglInitted())
    {
        const char* const GL_ERROR_MSG = "Error initializing GL extensions. OpenGL 2.0 or later is required.";
        exit(EXIT_FAILURE);
    }
#endif

#ifndef __EMSCRIPTEN__
    atexit(teardown);
#endif

    std::string storagePath;
#ifdef _WIN32
    storagePath = getenv("APPDATA");
#elif defined(__EMSCRIPTEN__)
    storagePath = "";
#elif defined(ANDROID)
    storagePath = SDL_AndroidGetExternalStoragePath();
#else
    storagePath = getenv("HOME");
#endif
    storagePath += "/.reminecraftpe";

    if (!storagePath.empty())
        createFolderIfNotExists(storagePath.c_str());

    g_pApp = new NinecraftApp;
    g_pApp->m_externalStorageDir = storagePath;
    g_pAppPlatform = new UsedAppPlatform(g_pApp->m_externalStorageDir, screen);
    g_pApp->m_pPlatform = g_pAppPlatform;
    g_pApp->init();

    resize();

    while (true)
    {
        main_loop();
    }

    return 0;
}
