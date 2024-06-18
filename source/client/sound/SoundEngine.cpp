/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem)
{
	field_40 = 0;
	field_A1C = 0;
	m_pOptions = nullptr;
	field_A20 = 0;
	m_pSoundSystem = soundSystem;
}

void SoundEngine::init(Options* options, AppPlatform* platform)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	m_pOptions = options;
	// Load Sounds
	SoundDesc::_load(platform);

#define SOUND(category, name) m_repository.add(category, SA_##name);
#include "sound_list.h"
#undef SOUND
}

void SoundEngine::play(const std::string& name)
{
	if (m_pOptions->m_fMasterVolume == 0.0f)
		return;

	SoundDesc sd;

	if (m_repository.get(name, sd)) {
		m_pSoundSystem->playAt(sd, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	}
}

void SoundEngine::play(const std::string& name, float x, float y, float z, float volume, float pitch)
{
	if (m_pOptions->m_fMasterVolume == 0.0f || volume <= 0.0f)
		return;

	SoundDesc sd;

	if (m_repository.get(name, sd)) {
		m_pSoundSystem->playAt(sd, x, y, z, volume, pitch);
	}
}
