#pragma once

#if (defined(_WIN32) || defined(USE_SDL1))
    #ifdef _WIN32
        #ifndef __clang__
            #ifdef USE_SDL1
                #pragma comment(lib, "SDL.lib")
            #else
                #pragma comment(lib, "SDL2.lib")
            #endif
        #endif
        #include <SDL_opengl.h>
    #else
        #include <SDL/SDL_opengl.h>
    #endif
#else
    #include <SDL2/SDL_opengl.h>
#endif