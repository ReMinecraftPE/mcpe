/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"

SoundEngine::SoundEngine()
{
#ifndef ORIGINAL_CODE
	m_pOptions = nullptr;
	field_A20 = 0;
#endif
}

#ifndef ORIGINAL_CODE
#define INIT_ARGS Options* pOptions, AppPlatform* pAppPlatform
#else
#define INIT_ARGS Options* pOptions
#endif

void SoundEngine::init(INIT_ARGS)
{
	m_pOptions = pOptions;

#ifdef USE_PCM_FILES
	SA_cloth1 = pAppPlatform->loadSound("sounds/cloth1.pcm");
	SA_cloth2 = pAppPlatform->loadSound("sounds/cloth2.pcm");
	SA_cloth3 = pAppPlatform->loadSound("sounds/cloth3.pcm");
	SA_cloth4 = pAppPlatform->loadSound("sounds/cloth4.pcm");

	SA_grass1 = pAppPlatform->loadSound("sounds/grass1.pcm");
	SA_grass2 = pAppPlatform->loadSound("sounds/grass2.pcm");
	SA_grass3 = pAppPlatform->loadSound("sounds/grass3.pcm");
	SA_grass4 = pAppPlatform->loadSound("sounds/grass4.pcm");

	SA_gravel1 = pAppPlatform->loadSound("sounds/gravel1.pcm");
	SA_gravel2 = pAppPlatform->loadSound("sounds/gravel2.pcm");
	SA_gravel3 = pAppPlatform->loadSound("sounds/gravel3.pcm");
	SA_gravel4 = pAppPlatform->loadSound("sounds/gravel4.pcm");

	SA_sand1 = pAppPlatform->loadSound("sounds/sand1.pcm");
	SA_sand2 = pAppPlatform->loadSound("sounds/sand2.pcm");
	SA_sand3 = pAppPlatform->loadSound("sounds/sand3.pcm");
	SA_sand4 = pAppPlatform->loadSound("sounds/sand4.pcm");

	SA_stone1 = pAppPlatform->loadSound("sounds/stone1.pcm");
	SA_stone2 = pAppPlatform->loadSound("sounds/stone2.pcm");
	SA_stone3 = pAppPlatform->loadSound("sounds/stone3.pcm");
	SA_stone4 = pAppPlatform->loadSound("sounds/stone4.pcm");

	SA_wood1 = pAppPlatform->loadSound("sounds/wood1.pcm");
	SA_wood2 = pAppPlatform->loadSound("sounds/wood2.pcm");
	SA_wood3 = pAppPlatform->loadSound("sounds/wood3.pcm");
	SA_wood4 = pAppPlatform->loadSound("sounds/wood4.pcm");

	SA_splash = pAppPlatform->loadSound("sounds/splash.pcm");
	SA_explode = pAppPlatform->loadSound("sounds/explode.pcm");
	SA_click = pAppPlatform->loadSound("sounds/click.pcm");
#endif

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
}

void SoundEngine::play(const std::string& name)
{
	if (m_pOptions->field_4 == 0.0f)
		return;

	SoundDesc sd;

	if (m_repository.get(name, sd))
		m_soundSystem.playAt(sd, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

void SoundEngine::play(const std::string& name, float a, float b, float c, float d, float e)
{
	if (m_pOptions->field_4 == 0.0f || d <= 0.0f)
		return;

	SoundDesc sd;

	if (m_repository.get(name, sd))
		m_soundSystem.playAt(sd, a, b, c, d, e);
}
