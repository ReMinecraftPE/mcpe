/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <stdint.h>
#include "client/app/AssetFile.hpp"

class AppPlatform;

class AudioCodec
{
public:
	enum Type
	{
		NONE,
		PCM,
		OGG
	};
};

struct PCMSoundHeader
{
	int m_channels;
	int m_bytes_per_sample;
	int m_sample_rate;
	int m_length;
};

struct SoundBuffer
{
	void* m_pData;
	int m_dataSize;
};

struct AudioDescriptor
{
    bool m_isLoaded;
    AudioCodec::Type m_codecType;
    PCMSoundHeader m_header;
};

#define SOUND_DIRS_SIZE 3

struct SoundDesc : AudioDescriptor
{
	static std::string dirs[SOUND_DIRS_SIZE];

	AssetFile m_file;
	SoundBuffer m_buffer;
	unsigned char* m_fileData;

	bool _load(const AppPlatform* platform, const char* category, const char *name);
	bool _loadPcm(const AppPlatform* platform, const char *name);
	bool _loadOgg(const AppPlatform* platform, const char* category, const char *name);
	void _unload();
	static void _loadAll(const AppPlatform*);
	static void _unloadAll();
};

#define SOUND(category, name, number) extern SoundDesc SA_##name##number;
#include "sound_list.h"
#undef SOUND
