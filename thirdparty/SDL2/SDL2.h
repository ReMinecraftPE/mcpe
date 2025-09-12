#pragma once

#if (defined(_WIN32) || defined(__DREAMCAST__))

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