#pragma once

#ifdef _WIN32
#pragma comment(lib, "SDL2.lib")
#include <SDL_opengl.h>
#else
#include <SDL2/SDL_opengl.h>
#endif