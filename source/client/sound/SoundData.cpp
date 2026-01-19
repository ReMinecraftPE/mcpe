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

bool SoundDesc::_load(Options* options, const AppPlatform* platform, const char* category, const char *name)
{
	if (m_isLoaded)
	{
		// Already Loaded
		LOG_W("Sound \"%s\" is already loaded!", name);
        return true;
	}

    // Load
	const std::vector<std::string>& resourcepacks = options->m_resourcepacks;
	std::string packdir, slashname = "/" + std::string(name);
	bool ret = false;

	// try to use the resource pack version of the sound
	for (size_t i = 0; i < resourcepacks.size(); ++i)
	{
        packdir = "/resource_packs/" + resourcepacks[i] + "/";
        for (size_t i = 0; i < SOUND_DIRS_SIZE; ++i)
        {
            ret = _loadOgg(platform, (packdir + dirs[i] + "/" + category + "/" + name + ".ogg").c_str());
            if (ret)
                return ret;
        }
		ret = _loadPcm(platform, (packdir + "sound/" + name + ".pcm").c_str());
		if (ret)
			return ret;
	}
	// no active resource packs have the sound, use the vanilla one or no sound
    for (size_t i = 0; i < SOUND_DIRS_SIZE; ++i)
    {
        ret = _loadOgg(platform, (dirs[i] + "/" + category + "/" + name + ".ogg").c_str());
        if (ret)
            return ret;
    }
    ret = _loadPcm(platform, ("sound/" + std::string(name) + ".pcm").c_str());
    if (!ret) {
        m_codecType = AudioCodec::NONE;
        LOG_W("Failed to load sound \"%s\"!", name);
        return false;
    } else
        return true;
}

bool SoundDesc::_loadPcm(const AppPlatform* platform, const char *name)
{
    m_file = platform->readAssetFile(name, true);
    m_isLoaded = m_file.size > 0;

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::PCM;
    m_header = *(PCMSoundHeader *) m_file.data;
    m_buffer.m_pData = (int16_t *) (m_file.data + sizeof(PCMSoundHeader));
    m_buffer.m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;

    // Success!
    return true;
}

bool SoundDesc::_loadOgg(const AppPlatform* platform, const char *name)
{
    m_file = platform->readAssetFile(name, true);
    m_isLoaded = m_file.size > 0;

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::OGG;
    m_header.m_bytes_per_sample = 2; // Always 2 (16-bit)
    m_header.m_length = stb_vorbis_decode_memory(m_file.data, (int) m_file.size, &m_header.m_channels, &m_header.m_sample_rate, &m_buffer.m_pData);

    delete[] m_file.data;
    m_file.data = nullptr;

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
    if (m_codecType == AudioCodec::OGG)
    {
        free(m_buffer.m_pData);
    }

    // Free File Data
    delete[] m_file.data;

    m_isLoaded = false;
}

// Load All Sounds
void SoundDesc::_loadAll(const AppPlatform* platform, Options* options)
{
#define SOUND(category, name) SA_##name._load(options, platform, #category, #name);
#define SOUND_NUM(category, name, number) SA_##name##number._load(options, platform, #category, #name#number);
#include "sound_list.h"
#undef SOUND
#undef SOUND_NUM
}

// Un-load All Sounds
void SoundDesc::_unloadAll()
{
#define SOUND(category, name) SA_##name._unload();
#define SOUND_NUM(category, name, number) SA_##name##number._unload();
#include "sound_list.h"
#undef SOUND
#undef SOUND_NUM
}

// Declare Variables
#define SOUND(category, name) SoundDesc SA_##name;
#define SOUND_NUM(category, name, number) SoundDesc SA_##name##number;
#include "sound_list.h"
#undef SOUND
#undef SOUND_NUM
