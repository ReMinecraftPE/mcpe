#include <cstdarg>

#include <SDL2/SDL.h>

#include "compat/GL.hpp"
#include "compat/AKeyCodes.hpp"
#include "App.hpp"
#include "AppPlatform_sdl.hpp"
#include "NinecraftApp.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

void LogMsg(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);

	vprintf(fmt, lst);
	printf("\n");

	va_end(lst);
}
// I hate duplicating code, but yeah
void LogMsgNoCR(const char* fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);

	vprintf(fmt, lst);

	va_end(lst);
}

AppPlatform_sdl *g_AppPlatform;
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
				if (event.key.keysym.sym == SDLK_F2)
				{
					if (event.key.state == SDL_PRESSED && g_AppPlatform != nullptr)
					{
						g_AppPlatform->saveScreenshot("", -1, -1);
					}
					break;
				}
				Keyboard::feed(event.key.state == SDL_PRESSED ? 1 : 0, translate_sdl_key_to_mcpe(event.key.keysym.sym));
				if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				{
					g_AppPlatform->setShiftPressed(event.key.state == SDL_PRESSED, event.key.keysym.sym == SDLK_LSHIFT);
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				Mouse::feed(event.button.button == SDL_BUTTON_LEFT ? 1 : 2, event.button.state == SDL_PRESSED ? 1 : 0, event.button.x, event.button.y);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				Mouse::_x = event.motion.x;
				Mouse::_y = event.motion.y;
				Mouse::feed(0, 0, event.motion.x, event.motion.y);
				g_AppPlatform->setMouseDiff(event.motion.xrel, event.motion.yrel);
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				Mouse::feed(3, event.wheel.y, Mouse::_x, Mouse::_y);
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

// GUI Scale
static void calulate_gui_scale()
{
	int width = Minecraft::width;

	// Modified Version Of https://github.com/MCPI-Revival/Ninecraft/blob/3f71638a10b581f6a50669edb24bc1ef1a92fbea/ninecraft/src/main.c#L243-L255
	if (width < 1000)
	{
		if (width < 400)
		{
			Gui::InvGuiScale = 1.0;
		}
		else
		{
			Gui::InvGuiScale = 0.5;
		}
	}
	else
	{
		Gui::InvGuiScale = 0.25;
	}
}

// Resizing
static void resize()
{
	SDL_GL_GetDrawableSize(window, &Minecraft::width, &Minecraft::height);

	calulate_gui_scale();

	if (g_pApp != nullptr && g_pApp->m_pScreen != nullptr)
	{
		g_pApp->m_pScreen->setSize(Minecraft::width * Gui::InvGuiScale, Minecraft::height * Gui::InvGuiScale);
	}
}

// Main Loop
#ifndef __EMSCRIPTEN__
#define EM_BOOL bool
#define EM_TRUE true
#define EM_FALSE false
#endif
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
		delete g_pApp;
		delete g_AppPlatform;
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGE("Unable To Initialize SDL: %s\n", SDL_GetError());
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
	window = SDL_CreateWindow("ReMinecraftPE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Minecraft::width, Minecraft::height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		LOGE("Unable To Create SDL Window\n");
		exit(EXIT_FAILURE);
	}

	// Enable Text Input
	SDL_StartTextInput();

	// Create OpenGL ES Context
	context = SDL_GL_CreateContext(window);
	if (!context)
	{
		LOGE("Unable To Create OpenGL Context\n");
		exit(EXIT_FAILURE);
	}

	// Setup Compatibility Layer If Needed
#ifdef USE_GLES1_COMPATIBILITY_LAYER
	init_gles_compatibility_layer();
#endif

	// Setup Teardown
#ifndef __EMSCRIPTEN__
	atexit(teardown);
#endif

	// Set Size
	resize();

	// Storage Directory
#ifdef _WIN32
	std::string storagePath = getenv("APPDATA");
#elif defined(__EMSCRIPTEN__)
	std::string storagePath = "";
#else
	std::string storagePath = getenv("HOME");
#endif
	storagePath += "/.reminecraftpe";
	ensure_screenshots_folder(storagePath.c_str());

	// Start MCPE
	g_pApp = new NinecraftApp;
	g_pApp->m_externalStorageDir = storagePath;
	g_AppPlatform = new AppPlatform_sdl(g_pApp->m_externalStorageDir, window);
	g_pApp->m_pPlatform = g_AppPlatform;
	g_pApp->init();

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
