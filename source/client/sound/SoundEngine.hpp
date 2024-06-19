/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/options/Options.hpp"
#include "common/Random.hpp"
#include "SoundSystem.hpp"
#include "SoundRepository.hpp"

class SoundEngine
{
public:
	SoundEngine(SoundSystem* soundSystem);
	void init(Options*, AppPlatform*);
	void play(const std::string& name);
	void play(const std::string& name, float x, float y, float z, float volume, float pitch);

public:
	SoundSystem* m_pSoundSystem;
	Options* m_pOptions;
	int field_40;
	Random m_random;
	SoundRepository m_repository;
	int field_A1C;
	int field_A20;
};

