/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundSystem.hpp"

bool SoundSystem::isAvailable()
{
	return false;
}

void SoundSystem::setListenerPos(float x, float y, float z)
{
}

#ifndef ORIGINAL_CODE
void SoundSystem::setListenerAngle(float yaw, float pitch)
{
}
#else
void SoundSystem::setListenerAngle(float yaw)
{
}
#endif

void SoundSystem::load(const std::string& sound)
{
}

void SoundSystem::play(const std::string& sound)
{
}

void SoundSystem::pause(const std::string& sound)
{
}

void SoundSystem::stop(const std::string& sound)
{
}

void SoundSystem::playAt(const SoundDesc& sound, float x, float y, float z, float a, float b)
{
}
