/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundData.hpp"

#include "client/app/AppPlatform.hpp"

void SoundDesc::_load(const AppPlatform* platform, const char *name)
{
	if (m_isLoaded)
	{
		// Already Loaded
		return;
	}
	// Read Asset File
	m_file = platform->readAssetFile(std::string("sound/") + name + ".pcm");
	m_isLoaded = m_file.size > 0;
	if (m_isLoaded)
	{
		m_fileData = m_file.data;
		m_header = *(PCMSoundHeader *) m_fileData;
		m_pData = (uint16_t *) (m_fileData + sizeof(PCMSoundHeader));
		field_4 = m_header.m_channels * m_header.m_length * m_header.m_bytes_per_sample;
	}
}

// Load All Sounds
void SoundDesc::_load(const AppPlatform* platform)
{
#define SOUND(category, name) SA_##name._load(platform, #name);
#include "sound_list.h"
#undef SOUND
}

#define SOUND(category, name) SoundDesc SA_##name;
#include "sound_list.h"
#undef SOUND