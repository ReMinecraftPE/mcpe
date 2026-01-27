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
#include "client/resources/Resource.hpp"
#include "client/resources/ResourcePackManager.hpp"

std::string SoundDesc::dirs[] = {
    "sound",
    "newsound",
    "sound3"
};

bool SoundDesc::_load(const char* category, const char *name)
{
	if (m_isLoaded)
	{
		// Already Loaded
		LOG_W("Sound \"%s\" is already loaded!", name);
        return true;
	}

    ResourceLoader* pLoader = Resource::getLoader(ResourceLocation::USER_PACKAGE);
    if (!pLoader)
    {
        LOG_E("No ResourcePackManager singleton available!");
        throw std::bad_cast();
    }
    ResourcePackManager* loader = (ResourcePackManager*)pLoader;

    // Load
    std::string packdir;
	bool ret = false;
    ResourceLocation location;

    // @HACK: We look for multiple different audio files in multiple different directories.
    // We should always do our proper checks before falling back to a different resource pack.
    // That's why this look so ungodly ugly.

    if (loader->m_pPacks)
    {
        // try to use the resource pack version of the sound
        location.fileSystem = ResourceLocation::EXTERNAL_DIR;
        for (size_t i = 0; i < loader->m_pPacks->size(); ++i)
        {
            packdir = "resource_packs/" + (*loader->m_pPacks)[i] + "/";
            for (size_t i = 0; i < SOUND_DIRS_SIZE; ++i)
            {
                location.path = packdir + dirs[i] + "/" + category + "/" + name + ".ogg";
                ret = _loadOgg(location);
                if (ret)
                    return ret;
            }
            location.path = packdir + "sound/" + name + ".pcm";
            ret = _loadPcm(location);
            if (ret)
                return ret;
        }
    }

	// no active resource packs have the sound, use the vanilla one or no sound
    location.fileSystem = ResourceLocation::APP_PACKAGE;
    for (size_t i = 0; i < SOUND_DIRS_SIZE; ++i)
    {
        location.path = dirs[i] + "/" + category + "/" + name + ".ogg";
        ret = _loadOgg(location);
        if (ret)
            return ret;
    }
    location.path = "sound/" + std::string(name) + ".pcm";
    ret = _loadPcm(location);
    if (!ret) {
        m_codecType = AudioCodec::NONE;
        LOG_W("Failed to load sound \"%s\"!", name);
        return false;
    } else
        return true;
}

bool SoundDesc::_loadPcm(const ResourceLocation& location)
{
    Resource::load(location, m_stream);
    m_isLoaded = !m_stream.empty();

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::PCM;
    m_header = *(PCMSoundHeader *) m_stream.data();
    m_buffer.m_pData = (int16_t *) (m_stream.data() + sizeof(PCMSoundHeader));
    m_buffer.m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;

    // Success!
    return true;
}

bool SoundDesc::_loadOgg(const ResourceLocation& location)
{
    Resource::load(location, m_stream);
    m_isLoaded = !m_stream.empty();

    // Error
    if (!m_isLoaded) return false;

    m_codecType = AudioCodec::OGG;
    m_header.m_bytes_per_sample = 2; // Always 2 (16-bit)
    m_header.m_length = stb_vorbis_decode_memory((const unsigned char*)m_stream.data(), m_stream.size(), &m_header.m_channels, &m_header.m_sample_rate, &m_buffer.m_pData);

    m_stream.clear();

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
    m_stream.clear();

    m_isLoaded = false;
}

// Load All Sounds
void SoundDesc::_loadAll()
{
#define SOUND(category, name) SA_##name._load(#category, #name);
#define SOUND_NUM(category, name, number) SA_##name##number._load(#category, #name#number);
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
