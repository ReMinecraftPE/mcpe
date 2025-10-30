/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb_image/include/stb_vorbis.c"

#include "SoundData.hpp"

#include "common/Logger.hpp"
#include "client/app/AppPlatform.hpp"

std::string SoundDesc::dirs[] = {
    "sound",
    "newsound",
    "sound3"
};

bool SoundDesc::_load(const AppPlatform* platform, const char* category, const char *name)
{
	if (m_isLoaded)
	{
		// Already Loaded
		LOG_W("Sound \"%s\" is already loaded!", name);
        return true;
	}

    // Load
    if (_loadOgg(platform, category, name) || _loadPcm(platform, name))
    {
        // Success!
        return true;
    }
    else
    {
        m_codecType = AudioCodec::NONE;
        LOG_W("Failed to load sound \"%s\"!", name);
        return false;
    }
}

bool SoundDesc::_loadPcm(const AppPlatform* platform, const char *name)
{
    m_file = platform->readAssetFile(std::string("sound/") + name + ".pcm", true);
    m_isLoaded = m_file.size > 0;

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::PCM;
    m_fileData = m_file.data;
    m_header = *(PCMSoundHeader *) m_fileData;
    m_buffer.m_pData = (void *) (m_fileData + sizeof(PCMSoundHeader));
    m_buffer.m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;

    // Success!
    return true;
}

bool SoundDesc::_loadOgg(const AppPlatform* platform, const char* category, const char *name)
{
    for (int i = 0; i < SOUND_DIRS_SIZE; i++)
    {
        m_file = platform->readAssetFile(dirs[i] + '/' + category + '/' + name + ".ogg", true);
        m_isLoaded = m_file.size > 0;
        if (m_isLoaded) break;
    }

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::OGG;
    m_fileData = m_file.data;
    m_header.m_bytes_per_sample = 2; // Always 2 (16-bit)
    // Casting to a short** here might cause problems. Let's find out...
    // Seems like it doesn't. Cool.
    m_header.m_length = stb_vorbis_decode_memory(m_file.data, (int) m_file.size, &m_header.m_channels, &m_header.m_sample_rate, (short **) &m_buffer.m_pData);
    if (m_header.m_length == -1)
    {
        LOG_E("An error occurred while trying to decode a sound!");
        return false;
    }
    m_buffer.m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;

    // Success!
    return true;
}

void SoundDesc::_unload()
{
    if (!m_isLoaded)
    {
        // Sound is already unloaded
        return;
    }
    // Free OGG Data
    if (m_codecType == AudioCodec::OGG) {
        free(m_buffer.m_pData);
    }
    // Free File Data
    delete m_file.data;
    m_isLoaded = false;
}

// Load All Sounds
void SoundDesc::_loadAll(const AppPlatform* platform)
{
#define SOUND(category, name, number) SA_##name##number._load(platform, #category, #name#number);
#include "sound_list.h"
#undef SOUND
}

// Un-load All Sounds
void SoundDesc::_unloadAll()
{
#define SOUND(category, name, number) SA_##name##number._unload();
#include "sound_list.h"
#undef SOUND
}

// Declare Variables
#define SOUND(category, name, number) SoundDesc SA_##name##number;
#include "sound_list.h"
#undef SOUND