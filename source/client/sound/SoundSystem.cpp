/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundSystem.hpp"

SoundSystem::~SoundSystem()
{
}

bool SoundSystem::isAvailable()
{
	return false;
}

void SoundSystem::setListenerPos(const Vec3& pos)
{
}

void SoundSystem::setListenerAngle(const Vec2& rot)
{
}

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

void SoundSystem::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
}

void SoundSystem::startEngine()
{
}

void SoundSystem::stopEngine()
{
}

void SoundSystem::muteAudio()
{
}

void SoundSystem::unMuteAudio()
{
}