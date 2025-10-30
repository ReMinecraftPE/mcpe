#include <stdarg.h>

#include "thirdparty/SDL/SDL.h"

#include "thirdparty/GL/GL.hpp"
#include "client/app/App.hpp"

#ifdef __EMSCRIPTEN__
#include "emscripten/AppPlatform_sdl2_emscripten.hpp"
typedef AppPlatform_sdl2_emscripten UsedAppPlatform;
#else
#include "desktop/AppPlatform_sdl2_desktop.hpp"
typedef AppPlatform_sdl2_desktop UsedAppPlatform;
#endif

#include "client/app/NinecraftApp.hpp"
#include "client/player/input/Multitouch.hpp"

// Video Mode Flags
#define VIDEO_FLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)

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

// Touch
#define TOUCH_IDS_SIZE (MAX_TOUCHES - 1) // ID 0 Is Reserved For The Mouse
struct touch_id_data {
	bool active;
	SDL_TouchID device;
	SDL_FingerID finger;
    
    touch_id_data()
    {
        active = false;
    }
};
static touch_id_data touch_ids[TOUCH_IDS_SIZE];
static char get_touch_id(SDL_TouchID device, SDL_FingerID finger) {
	for (int i = 0; i < TOUCH_IDS_SIZE; i++) {
		touch_id_data &data = touch_ids[i];
		if (data.active && data.device == device && data.finger == finger) {
			return i + 1;
		}
	}
	// Not Found
	for (int i = 0; i < TOUCH_IDS_SIZE; i++) {
		// Find First Inactive ID, And Activate It
		touch_id_data &data = touch_ids[i];
		if (!data.active) {
			data.active = true;
			data.device = device;
			data.finger = finger;
			return i + 1;
		}
	}
	// Fail
	return 0;
}
static void drop_touch_id(int id) {
	touch_ids[id - 1].active = false;
}
static void handle_touch(int x, int y, int type, char id) {
	if (id == 0) {
		return;
	}
	switch (type) {
		case SDL_FINGERDOWN:
		case SDL_FINGERUP: {
			bool data = type == SDL_FINGERUP ? 0 : 1;
			Mouse::feed(BUTTON_LEFT, data, x, y);
			Multitouch::feed(BUTTON_LEFT, data, x, y, id);
			if (type == SDL_FINGERUP) {
				drop_touch_id(id);
			}
			break;
		}
		case SDL_FINGERMOTION: {
			Mouse::feed(BUTTON_NONE, 0, x, y);
			Multitouch::feed(BUTTON_NONE, 0, x, y, id);
			break;
		}
	}
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
				// Android Back Button
				if (event.key.keysym.sym == SDLK_AC_BACK)
				{
					g_pApp->handleBack(event.key.state == SDL_PRESSED);
					break;
				}

				// Text Editing
				if (event.key.keysym.sym == SDLK_BACKSPACE && event.key.state == SDL_PRESSED)
				{
					g_pApp->handleCharInput('\b');
				}

				g_pAppPlatform->handleKeyEvent(event);
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			{
				// Hate this hack
				if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START && event.cbutton.state == SDL_PRESSED)
				{
					g_pApp->pauseGame() || g_pApp->resumeGame();
				}
				g_pAppPlatform->handleControllerButtonEvent(event.cbutton.which, event.cbutton.button, event.cbutton.state);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.which != SDL_TOUCH_MOUSEID)
				{
					const float scale = g_fPointToPixelScale;
					MouseButtonType type = UsedAppPlatform::GetMouseButtonType(event.button.button);
					bool state = UsedAppPlatform::GetMouseButtonState(event);
					float x = event.button.x * scale;
					float y = event.button.y * scale;
					Mouse::feed(type, state, x, y);
					Multitouch::feed(type, state, x, y, 0);
				}
				break;
			}
			case SDL_MOUSEMOTION:
			{
				if (event.button.which != SDL_TOUCH_MOUSEID)
				{
					float scale = g_fPointToPixelScale;
					float x = event.motion.x * scale;
					float y = event.motion.y * scale;
					Multitouch::feed(BUTTON_NONE, false, x, y, 0);
					Mouse::feed(BUTTON_NONE, false, x, y);
					g_pAppPlatform->setMouseDiff(event.motion.xrel * scale, event.motion.yrel * scale);
				}
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				if (event.button.which != SDL_TOUCH_MOUSEID)
				{
					Mouse::feed(BUTTON_SCROLLWHEEL, UsedAppPlatform::GetMouseButtonState(event), Mouse::getX(), Mouse::getY());
				}
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
				g_pAppPlatform->handleControllerAxisEvent(event.caxis.which, event.caxis.axis, event.caxis.value);
				break;
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
			{
				float x = event.tfinger.x * Minecraft::width;
				float y = event.tfinger.y * Minecraft::height;
				handle_touch(x, y, event.type, get_touch_id(event.tfinger.touchId, event.tfinger.fingerId));
				break;
			}
			case SDL_TEXTINPUT:
			{
				if (g_pApp != nullptr)
				{
					size_t length = strlen(event.text.text);
					for (size_t i = 0; i < length; i++)
					{
						char x = event.text.text[i];
						if (x >= ' ' && x <= '~')
						{
							g_pApp->handleCharInput(x);
						}
					}
				}
				break;
			}
			case SDL_CONTROLLERDEVICEADDED:
				g_pAppPlatform->gameControllerAdded(event.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				g_pAppPlatform->gameControllerRemoved(event.cdevice.which);
				break;
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
	Minecraft::setRenderScaleMultiplier(g_fPointToPixelScale);
	
	// give it an update.
	// As said before, internally, this multiplies by the GUI scale multiplier
	if (g_pApp)
		g_pApp->sizeUpdate(windowWidth, windowHeight);
}

// Main Loop
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

// Main
int main(int argc, char *argv[])
{
	// Setup Logging
	Logger::setSingleton(new Logger);

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG_E("Unable To Initialize SDL: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Configure OpenGL ES Context
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#ifdef USE_GLES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

	// Lock To Landscape
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

	// Create Window
	window = SDL_CreateWindow("ReMinecraftPE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Minecraft::width, Minecraft::height, VIDEO_FLAGS);
	if (!window)
	{
		LOG_E("Unable to create SDL window: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Create OpenGL ES Context
	context = SDL_GL_CreateContext(window);
	if (!context)
	{
		const char* const GL_ERROR_MSG = "Unable to create OpenGL context";
		LOG_E(GL_ERROR_MSG);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL Error", GL_ERROR_MSG, window);
		exit(EXIT_FAILURE);
	}

    // Enable V-Sync
    // Not setting this explicitly results in undefined behavior
    if (SDL_GL_SetSwapInterval(-1) == -1) // Try adaptive
    {
        LOG_W("Adaptive V-Sync is not supported on this platform. Falling back to standard V-Sync...");
        // fallback to standard
		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			LOG_W("Setting the swap interval for V-Sync is not supported on this platform!");
		}
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
#elif defined(ANDROID)
	storagePath = SDL_AndroidGetExternalStoragePath();
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
