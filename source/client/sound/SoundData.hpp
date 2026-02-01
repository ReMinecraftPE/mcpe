/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <stdint.h>

struct ResourceLocation;

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
	int16_t *m_pData;
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
	static const std::string DIRS[SOUND_DIRS_SIZE];

	std::string m_stream;
	SoundBuffer m_buffer;

	bool _load(const char* category, const char *name);
	bool _loadPcm(const ResourceLocation& location);
	bool _loadOgg(const ResourceLocation& location);
	void _unload();
	static void _loadAll();
	static void _unloadAll();
};

#define SOUND(category, name) extern SoundDesc SA_##name;
#define SOUND_NUM(category, name, number) extern SoundDesc SA_##name##number;
#include "sound_list.h"
#undef SOUND
#undef SOUND_NUM
