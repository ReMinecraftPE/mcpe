#include <stdarg.h>

#ifdef XENON
#include <xenon_sound/sound.h>
#include <diskio/ata.h>
#include <input/input.h>
#include <time/time.h>

#include <libfat/fat.h>
#endif

#include "thirdparty/SDL/SDL.h"
#include "thirdparty/SDL/SDL_gamecontroller.h"

#include "client/app/App.hpp"
#include "desktop/AppPlatform_sdl1_desktop.hpp"
typedef AppPlatform_sdl1_desktop UsedAppPlatform;

#include "client/app/NinecraftApp.hpp"
#include "client/player/input/Multitouch.hpp"

#ifndef MCE_GFX_API_NULL
#define MCE_GFX_API_OGL 1
#include "renderer/platform/ogl/Extensions.hpp"
#endif

// Video Mode Flags
#if MCE_GFX_API_OGL
#define VIDEO_FLAGS (SDL_OPENGL | SDL_RESIZABLE)
#elif MC_PLATFORM_XBOX
#define VIDEO_FLAGS (0)
#else
#define VIDEO_FLAGS (SDL_RESIZABLE)
#endif

static float g_fPointToPixelScale = 1.0f;

UsedAppPlatform* g_pAppPlatform;
NinecraftApp* g_pApp;

SDL_Surface* screen = NULL;

static void initPlatform()
{
#ifdef XENON
    xenon_make_it_faster(XENON_SPEED_FULL);
    xenos_init(VIDEO_MODE_AUTO);
    xenon_sound_init();
    console_init();
    usb_init();
    usb_do_poll();

	xenon_ata_init();

	xenon_atapi_init();
	
	fatInitDefault();

    // disable buffering for stdout, so we don't lose our logs when abort is called
    setbuf(stdout, NULL);
    freopen("reminecraftpe.stdout.log", "w", stdout);
    freopen("reminecraftpe.stderr.log", "w", stderr);
#endif
}

static void preInitGraphics()
{
#if MCE_GFX_API_OGL
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
#endif
}

static void initGraphics()
{
#if MCE_GFX_API_OGL
    if (!mce::Platform::OGL::InitBindings())
    {
        LOG_E("Error initializing GL extensions. OpenGL 2.0 or later is required.");
        exit(EXIT_FAILURE);
    }
#endif
}

static std::string getStoragePath()
{
    // Doing this as a c-string because worst-case, an SDK
    // will return a nullptr instead of an empty string
    const char* pathBase;
#ifdef _WIN32
    pathBase = getenv("APPDATA");
#else
    pathBase = getenv("HOME");
#endif

    if (pathBase == nullptr || pathBase[0] == '\0')
        pathBase = ""; // just use the current working directory

    std::string path(pathBase);
    if (!path.empty())
        path += "/";
    path += ".reminecraftpe";

    return path;
}

static void teardown()
{
#ifdef XENON
    // Give the shitty flashdrive time to finish writing the log files
    delay(30);
#endif

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
                if (event.type == SDL_KEYDOWN && event.key.keysym.unicode > 0)
                {
                    if ((event.key.keysym.unicode & 0xFF80) == 0)
                    {
                        char ch = event.key.keysym.unicode & 0x7F;
                        g_pApp->handleCharInput(ch);
                    }
                    else
                    {
                        // An international character..
                    }
                }

                g_pAppPlatform->handleKeyEvent(event);
                break;
            }
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            {
                // Hate this hack
                if (event.jbutton.button == SDL_CONTROLLER_BUTTON_START && event.jbutton.state == SDL_PRESSED)
                {
                    g_pApp->pauseGame() || g_pApp->resumeGame();
                }
                g_pAppPlatform->handleControllerButtonEvent(event.jbutton.which, event.jbutton.button, event.jbutton.state);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                const float scale = g_fPointToPixelScale;
                MouseButtonType type = UsedAppPlatform::GetMouseButtonType(event.button.button);
                bool state = UsedAppPlatform::GetMouseButtonState(event);
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
            case SDL_JOYAXISMOTION:
                g_pAppPlatform->handleControllerAxisEvent(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
                break;
            case SDL_VIDEORESIZE:
            {
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, VIDEO_FLAGS);
                g_pApp->onGraphicsReset();
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

#if MCE_GFX_API_OGL
    SDL_GL_SwapBuffers();
#endif

    if (g_pApp->wantToQuit())
    {
        g_pApp->saveOptions();
        delete g_pApp;
        delete g_pAppPlatform;
        teardown();
        exit(EXIT_SUCCESS);
    }
}

// Main
int main(int argc, char* argv[])
{
    initPlatform();

    Logger::setSingleton(new Logger);

    //LOG_I("Initializing SDL...");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        LOG_E("Couldn't initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

	preInitGraphics();
    
    SDL_EnableUNICODE(SDL_TRUE);

	SDL_WM_SetCaption("ReMinecraftPE", nullptr);
    //LOG_I("Setting SDL video mode...");
    // XENON: width and height need to be accurate to what's already set by the console,
    // or else libXenon will crash.
    screen = SDL_SetVideoMode(Minecraft::width, Minecraft::height, 0, VIDEO_FLAGS);
    if (!screen)
    {
        LOG_E("Failed to set SDL video mode: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

#if MC_PLATFORM_XBOX
    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
#endif

	initGraphics();

    atexit(teardown);

    std::string storagePath = getStoragePath();
    if (!storagePath.empty())
        createFolderIfNotExists(storagePath.c_str());

    //LOG_I("Initializing AppPlatform...");
    g_pAppPlatform = new UsedAppPlatform(storagePath, screen);
    g_pAppPlatform->m_externalStorageDir = storagePath;
    //LOG_I("Initializing NinecraftApp...");
    g_pApp = new NinecraftApp;
    g_pApp->m_pPlatform = g_pAppPlatform;
    g_pApp->init();

    resize();
    // We're off to the races
    while (true)
    {
        // We have no way to debug with libXenon, and aborting will kill our logs
#ifdef XENON
        try
        {
            main_loop();
        }
        catch (...)
        {
            LOG_E("Encountered a runtime exception!");
            exit(EXIT_FAILURE);
        }
#else
        main_loop();
#endif
    }

    return 0;
}
