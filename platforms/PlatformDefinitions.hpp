/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

// Sound Systems
#include "client/sound/SoundSystem.hpp"

#if false

// Add sound system overrides here

#elif defined(USE_SDL)

// -- OpenAL based sound system for SDL
#include "sdl/SoundSystemAL.hpp"
#define SOUND_SYSTEM_TYPE SoundSystemAL

#elif defined(_WIN32)

// -- DirectSound based sound system for Windows
#include "windows/SoundSystemWindows.hpp"
#define SOUND_SYSTEM_TYPE SoundSystemWindows

#else

// -- Empty SoundSystem placeholder.
#define SOUND_SYSTEM_TYPE SoundSystem

#endif

// @TODO: SoundSystemSL - Use this for the Android port
