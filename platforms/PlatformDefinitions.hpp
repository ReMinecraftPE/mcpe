/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// @DEPRECATED: This is a stupid-ass file. Whoever wrote it didn't understand that each of the SoundSystems aren't going to have their own special functions.
//				Either rename it and move it into Sounds or remove it entirely if possible.

#pragma once

// Sound Systems
#include "client/sound/SoundSystem.hpp"

#if 0

// Add sound system overrides here

#elif defined(USE_OPENAL)

// -- OpenAL based sound system
#include "openal/SoundSystemAL.hpp"
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
