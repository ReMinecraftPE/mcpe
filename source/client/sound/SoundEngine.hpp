/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/common/Options.hpp"
#include "client/common/Random.hpp"
#include "SoundSystem.hpp"
#include "SoundRepository.hpp"

#include "platforms/PlatformDefinitions.hpp"

class SoundEngine
{
public:
	SoundEngine();
	void init(Options*);
	void play(const std::string& name);
	void play(const std::string& name, float, float, float, float, float);

public:
	SOUND_SYSTEM_TYPE m_soundSystem;
	Options* m_pOptions;
	int field_40 = 0;
	Random m_random;
	SoundRepository m_repository;
	int field_A1C = 0;
	int field_A20;
};

