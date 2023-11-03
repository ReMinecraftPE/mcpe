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

void SoundEngine::init(Options* options)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	m_pOptions = options;

#ifndef MISSING_SOUND_DATA
	m_repository.add("step.cloth", SA_cloth1);
	m_repository.add("step.cloth", SA_cloth2);
	m_repository.add("step.cloth", SA_cloth3);
	m_repository.add("step.cloth", SA_cloth4);

	m_repository.add("step.grass", SA_grass1);
	m_repository.add("step.grass", SA_grass2);
	m_repository.add("step.grass", SA_grass3);
	m_repository.add("step.grass", SA_grass4);

	m_repository.add("step.gravel", SA_gravel1);
	m_repository.add("step.gravel", SA_gravel2);
	m_repository.add("step.gravel", SA_gravel3);
	m_repository.add("step.gravel", SA_gravel4);

	m_repository.add("step.sand", SA_sand1);
	m_repository.add("step.sand", SA_sand2);
	m_repository.add("step.sand", SA_sand3);
	m_repository.add("step.sand", SA_sand4);

	m_repository.add("step.stone", SA_stone1);
	m_repository.add("step.stone", SA_stone2);
	m_repository.add("step.stone", SA_stone3);
	m_repository.add("step.stone", SA_stone4);

	m_repository.add("step.wood", SA_wood1);
	m_repository.add("step.wood", SA_wood2);
	m_repository.add("step.wood", SA_wood3);
	m_repository.add("step.wood", SA_wood4);

	m_repository.add("random.splash",  SA_splash);
	m_repository.add("random.explode", SA_explode);
	m_repository.add("random.click",   SA_click);
#endif
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
