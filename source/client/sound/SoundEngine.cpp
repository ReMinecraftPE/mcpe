/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"
#include "common/Mth.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem)
{
	m_pSoundSystem = soundSystem;
	m_pOptions = nullptr;
	field_40 = 0;
	field_A1C = 0;
	field_A20 = 0;
	m_muted = false;
}

float SoundEngine::_getVolumeMult(const Vec3& pos)
{
	// @TODO: this is not supposed to be a constant
	return 1.0f;
}

void SoundEngine::init(Options* options, AppPlatform* platform)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	m_pOptions = options;
	// Load Sounds
	SoundDesc::_load(platform);

#define SOUND(category, name) m_sounds.add(category, SA_##name);
#include "sound_list.h"
#undef SOUND
}

void SoundEngine::enable(bool b)
{
}

void SoundEngine::updateOptions()
{
}

void SoundEngine::mute()
{
	m_muted = true;
}

void SoundEngine::unMute()
{
	m_muted = false;
}

void SoundEngine::destroy()
{
}

void SoundEngine::play(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	float vol = m_pOptions->m_fMasterVolume * volume;
	if (vol <= 0.0f)
		return;

	float cVolume = Mth::clamp(_getVolumeMult(pos) * vol, 0.0f, 1.0f);
	float cPitch = Mth::clamp(pitch, -1.0f, 1.0f);
	SoundDesc sd;

	if (m_sounds.get(name, sd))
	{
		m_pSoundSystem->playAt(sd, pos.x, pos.y, pos.z, cVolume, pitch);
	}
}

void SoundEngine::playUI(const std::string& name, float volume, float pitch)
{
	volume *= 0.25F; // present on Java b1.2_02, but not Pocket for some reason
	float vol = m_pOptions->m_fMasterVolume * volume;
	if (vol <= 0.0f)
		return;

	float cVolume = Mth::clamp(vol, 0.0f, 1.0f);
	float cPitch = Mth::clamp(pitch, -1.0f, 1.0f);
	SoundDesc sd;

	if (m_sounds.get(name, sd))
	{
		m_pSoundSystem->playAt(sd, 0.0f, 0.0f, 0.0f, cVolume, pitch);
	}
}
