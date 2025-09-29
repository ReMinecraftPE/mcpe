#pragma once

#if (defined(_WIN32) || defined(USE_SDL1))

#ifdef _WIN32
#ifdef USE_SDL1
#pragma comment(lib, "SDL.lib")
#else
#pragma comment(lib, "SDL2.lib")
#endif
#include <SDL.h>
#include <SDL_syswm.h>
#else
#include <SDL/SDL.h>
#endif

#else
#include <SDL2/SDL.h>
#endif