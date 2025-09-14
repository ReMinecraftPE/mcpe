#pragma once

#if (defined(_WIN32) || SDL_MAJOR_VERSION == 1)

#ifdef _WIN32
#pragma comment(lib, "SDL2.lib")
#include <SDL.h>
#include <SDL_syswm.h>
#else
#include <SDL/SDL.h>
#endif

#else
#include <SDL2/SDL.h>
#endif