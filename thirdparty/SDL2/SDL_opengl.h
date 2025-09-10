#pragma once

#if (defined(_WIN32) || defined(__DREAMCAST__))

#ifdef _WIN32
#pragma comment(lib, "SDL2.lib")
#include <SDL_opengl.h>
#else
#include <SDL/SDL_opengl.h>
#endif

#else
#include <SDL2/SDL_opengl.h>
#endif