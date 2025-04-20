/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundData.hpp"

#include "thirdparty/stb_image/include/stb_vorbis.c"

#include "client/app/AppPlatform.hpp"

bool SoundDesc::_load(const AppPlatform* platform, const char* category, const char *name)
{
	if (m_isLoaded)
	{
        LOG_W("Sound \"%s\" is already loaded!", name);
        return true;
	}
    
    if (_loadOgg(platform, category, name) || _loadPcm(platform, category, name))
    {
        // Success!
        return true;
    }
    else
    {
        m_codecType = AudioCodec::NONE;
    }
    
    LOG_W("Failed to load sound \"%s\"!", name);
    return false;
}

bool SoundDesc::_loadPcm(const AppPlatform* platform, const char* category, const char *name)
{
    m_file = platform->readAssetFile(std::string("sound/") + name + ".pcm");
    m_isLoaded = m_file.size > 0;
    if (m_isLoaded)
    {
        m_codecType = AudioCodec::PCM;
        m_fileData = m_file.data;
        m_header = *(PCMSoundHeader *) m_fileData;
        m_pData = (uint16_t *) (m_fileData + sizeof(PCMSoundHeader));
        m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;
        
        // Success!
        return true;
    }
    
    return false;
}

bool SoundDesc::_loadOgg(const AppPlatform* platform, const char* category, const char *name)
{
    m_file = platform->readAssetFile(std::string("sound/") + category + '/' + name + ".ogg");
    m_isLoaded = m_file.size > 0;
    if (m_isLoaded)
    {
        m_codecType = AudioCodec::OGG;
        m_fileData = m_file.data;
        m_header.m_bytes_per_sample = 2; // Always 2 (16-bit)
        // Casting to a short** here might cause problems. Let's find out...
        // Seems like it doesn't. Cool.
        m_header.m_length = stb_vorbis_decode_memory(m_file.data, (int)m_file.size, &m_header.m_channels, &m_header.m_sample_rate, (short**)&m_pData);
        if (m_header.m_length == -1)
        {
            LOG_E("An error occurred while trying to decode a sound!");
            return false;
        }
        m_dataSize = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;
    
        // Success!
        return true;
    }
    
    return false;
}

void SoundDesc::_unload()
{
    if (!m_isLoaded)
    {
        // Sound is already unloaded
        return;
    }
    
    switch (m_codecType)
    {
        case AudioCodec::OGG:
            free(m_pData);
            break;
        default:
            // Do nothing
            break;
    }
    
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

#define SOUND(category, name, number) SoundDesc SA_##name##number;
#include "sound_list.h"
#undef SOUND
