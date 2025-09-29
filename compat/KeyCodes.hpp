/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#ifdef USE_SDL
    #include "thirdparty/SDL/SDL.h"

    // because SDL sucks and makes no sense and sets bit 1<<30 for some keycodes for some godamn reason
    enum eSDLVirtualKeys
    {
        #define CODE(x) SDLVK_ ## x,
        #include "SDLKeyCodes.h"
        #undef  CODE
    };
#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif __APPLE__
    // https://i.stack.imgur.com/LD8pT.png
    #define AKEYCODE_FORWARD_DEL 0x75
    #define AKEYCODE_ARROW_LEFT  0x7B
    #define AKEYCODE_ARROW_RIGHT 0x7C
    #define AKEYCODE_DEL         0x33
    #define AKEYCODE_ENTER       0x24
#endif

#ifdef USE_NATIVE_ANDROID
    #include <android/keycodes.h>

    #define AKEYCODE_ARROW_LEFT  AKEYCODE_DPAD_LEFT
    #define AKEYCODE_ARROW_RIGHT AKEYCODE_DPAD_RIGHT
#endif