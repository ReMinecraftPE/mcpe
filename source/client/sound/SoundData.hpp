/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "common/Utils.hpp"
#include "client/app/AssetFile.hpp"

struct AppPlatform;

struct PCMSoundHeader
{
	int m_channels;
	int m_bytes_per_sample;
	int m_sample_rate;
	int m_length;
};

struct SoundDesc
{
	bool m_isLoaded;
	AssetFile m_file;
	uint16_t* m_pData;
	int field_4;
	PCMSoundHeader m_header;
	unsigned char* m_fileData;

	void _load(const AppPlatform* platform, const char *name);
	static void _load(const AppPlatform*);
};

#define SOUND(category, name) extern SoundDesc SA_##name;
#include "sound_list.h"
#undef SOUND