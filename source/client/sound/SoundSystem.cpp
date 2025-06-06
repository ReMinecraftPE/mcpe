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

void SoundSystem::setListenerVelocity(const Vec3& vel)
{
}

void SoundSystem::setMusicVolume(float vol)
{
}

void SoundSystem::setSoundVolume(float vol)
{
}

void SoundSystem::load(const std::string& soundPath, bool is3D, float minDis)
{
}

void SoundSystem::play(const std::string& soundPath)
{
}

void SoundSystem::pause(const std::string& soundPath)
{
}

void SoundSystem::stop(const std::string& soundPath)
{
}

void SoundSystem::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
}

void SoundSystem::playMusic(const std::string& soundPath)
{
}

bool SoundSystem::isPlayingMusic() const
{
	return false;
}

bool SoundSystem::isPlayingMusic(const std::string& soundPath) const
{
	return false;
}

void SoundSystem::stopMusic()
{
}

void SoundSystem::pauseMusic(bool state)
{
}

void SoundSystem::update(float elapsedTime)
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