#include <cstdarg>

#include "thirdparty/SDL2/SDL2.h"

#include "thirdparty/GL/GL.hpp"
#include "client/app/App.hpp"

#ifdef __EMSCRIPTEN__
#include "emscripten/AppPlatform_sdl.hpp"
#else
#include "desktop/AppPlatform_sdl.hpp"
#endif
typedef AppPlatform_sdl UsedAppPlatform;

#include "client/app/NinecraftApp.hpp"

static float g_fPointToPixelScale = 1.0f;

UsedAppPlatform *g_pAppPlatform;
NinecraftApp *g_pApp;

SDL_Window *window = NULL;
SDL_GLContext context = NULL;
static void teardown()
{
	if (window != NULL)
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		window = NULL;
	}
}

static int TranslateSDLKeyCodeToVirtual(int sdlCode)
{
	switch (sdlCode) {
		#define CODE(x) case SDLK_ ## x: return SDLVK_ ## x;
		#include "compat/SDLKeyCodes.h"
		#undef  CODE
	}
	return SDLVK_UNKNOWN;
}

// Resize From JS
#ifdef __EMSCRIPTEN__
extern "C" void resize_from_js(int new_width, int new_height)
{
	SDL_SetWindowSize(window, new_width, new_height);
}
#endif

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
				// TODO: Shouldn't we be handling this in Keyboard?
				
				// We really should. We didn't add the f2 key anywhere -iProgramInCpp
				/*
				if (event.key.keysym.sym == SDLK_F2)
				{
					if (event.key.state == SDL_PRESSED && g_pAppPlatform != nullptr)
					{
						g_pAppPlatform->saveScreenshot("", -1, -1);
					}
					break;
				}
				*/
				
				Keyboard::feed(AppPlatform_sdl_base::GetKeyState(event), TranslateSDLKeyCodeToVirtual(event.key.keysym.sym));
				if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				{
					g_pAppPlatform->setShiftPressed(event.key.state == SDL_PRESSED, event.key.keysym.sym == SDLK_LSHIFT);
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				const float scale = g_fPointToPixelScale;
				Mouse::feed(AppPlatform_sdl_base::GetMouseButtonType(event), AppPlatform_sdl_base::GetMouseButtonState(event), event.button.x * scale, event.button.y * scale);
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
			case SDL_MOUSEWHEEL:
			{
				Mouse::feed(BUTTON_SCROLLWHEEL, AppPlatform_sdl_base::GetMouseButtonState(event), Mouse::getX(), Mouse::getY());
				break;
			}
			case SDL_TEXTINPUT:
			{
				if (g_pApp != nullptr)
				{
					char x = event.text.text[0];
					if (x >= ' ' && x <= '~')
					{
						g_pApp->handleCharInput(x);
					}
				}
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					window_resized = true;
				}
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
    int drawWidth, drawHeight;
    SDL_GL_GetDrawableSize(window,
        &drawWidth, &drawHeight);
    
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window,
        &windowWidth, &windowHeight);
    
	Minecraft::width  = drawWidth;
	Minecraft::height = drawHeight;
	
	// recalculate the point to pixel scale.
	// This currently assumes that the aspect ratio is the same.
	g_fPointToPixelScale = float(drawWidth) / float(windowWidth);
	
	// Update the scale multiplier. We use the same value, because we pass to `sizeUpdate`, the window width/height.
	// They will be multiplied by the GUI scale multiplier, becoming the drawwidth and drawheight, times the decided on GUI scale.
	Minecraft::setGuiScaleMultiplier(g_fPointToPixelScale);
	
	// give it an update.
	// As said before, internally, this multiplies by the GUI scale multiplier
	if (g_pApp)
		g_pApp->sizeUpdate(windowWidth, windowHeight);
}

// Main Loop
static bool is_first_window_resize = true;
static EM_BOOL main_loop(double time, void *user_data)
{
	// Handle Events
	handle_events();

	// Screen Size
	if (window_resized)
	{
		window_resized = false;
		resize();
	}

	// Update MCPE
	g_pApp->update();

	// Swap Buffers
	SDL_GL_SwapWindow(window);

	if (g_pApp->wantToQuit())
	{
		g_pApp->saveOptions();
		delete g_pApp;
		delete g_pAppPlatform;
		teardown();
		// Stop Looping
		return EM_FALSE;
	}
	else
	{
		// Keep Looping
		return EM_TRUE;
	}
}

extern bool g_bIsMenuBackgroundAvailable; // client/gui/Screen.cpp
extern bool g_bAreCloudsAvailable;        // client/renderer/LevelRenderer.cpp

#ifdef __EMSCRIPTEN__
bool DoesAssetExist(const std::string & fileName)
{
    std::string realPath = g_pAppPlatform->getAssetPath(fileName);
	int width = 0, height = 0;
	char *data = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &width, &height);
	if (data == NULL)
		return false;
	
	free(data);
	return true;
}
#else
// access works just fine on linux and friends
#define DoesAssetExist(fileName) (XPL_ACCESS("assets/" fileName, 0) == 0)
#endif

void CheckOptionalTextureAvailability()
{
	g_bIsMenuBackgroundAvailable = DoesAssetExist("gui/background/panorama_0.png");
	g_bAreCloudsAvailable        = DoesAssetExist("environment/clouds.png");
}

// Main
int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG_E("Unable To Initialize SDL: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Configure OpenGL ES Context
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#ifdef USE_GLES1_COMPATIBILITY_LAYER
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
	// Double-Buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Window Size
#ifdef __EMSCRIPTEN__
	Minecraft::width = std::stoi(argv[1]);
	Minecraft::height = std::stoi(argv[2]);
#endif

	// Create Window
	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	window = SDL_CreateWindow("ReMinecraftPE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Minecraft::width, Minecraft::height, flags);
	if (!window)
	{
		LOG_E("Unable to create SDL window");
		exit(EXIT_FAILURE);
	}

	// Enable Text Input
	SDL_StartTextInput();

	// Create OpenGL ES Context
	context = SDL_GL_CreateContext(window);
	if (!context)
	{
		const char* const GL_ERROR_MSG = "Unable to create OpenGL context";
		LOG_E(GL_ERROR_MSG);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL Error", GL_ERROR_MSG, window);
		exit(EXIT_FAILURE);
	}

#ifdef _WIN32
	xglInit();

	if (!xglInitted())
	{
		const char* const GL_ERROR_MSG = "Error initializing GL extensions. OpenGL 2.0 or later is required. Update your graphics drivers!";
		LOG_E(GL_ERROR_MSG);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL Error", GL_ERROR_MSG, window);
		exit(EXIT_FAILURE);
	}
#endif

	// Setup Compatibility Layer If Needed
#ifdef USE_GLES1_COMPATIBILITY_LAYER
	init_gles_compatibility_layer();
#endif

	// Setup Teardown
#ifndef __EMSCRIPTEN__
	atexit(teardown);
#endif

	// Storage Directory
    std::string storagePath;
#ifdef _WIN32
	storagePath = getenv("APPDATA");
#elif defined(__EMSCRIPTEN__)
	storagePath = "";
#else
	storagePath = getenv("HOME");
#endif
	storagePath += "/.reminecraftpe";
    
	if (!storagePath.empty())
		createFolderIfNotExists(storagePath.c_str());
	
	// Start MCPE
	g_pApp = new NinecraftApp;
	g_pApp->m_externalStorageDir = storagePath;
	g_pAppPlatform = new UsedAppPlatform(g_pApp->m_externalStorageDir, window);
	g_pApp->m_pPlatform = g_pAppPlatform;
	g_pApp->init();

	CheckOptionalTextureAvailability();
    
    // Set Size
    resize();

	// Loop
#ifndef __EMSCRIPTEN__
	while (true)
	{
		EM_BOOL result = main_loop(0, nullptr);
		if (result == EM_FALSE)
		{
			break;
		}
	}
#else
	emscripten_request_animation_frame_loop(main_loop, nullptr);
#endif

	return 0;
}
