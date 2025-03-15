#pragma once

#ifdef _WIN32
#pragma comment(lib, "SDL2.lib")
#include <SDL.h>
#include <SDL_syswm.h>
#else
#include <SDL2/SDL.h>
#endif