/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CustomSoundSystem.hpp"

SoundSystemNL::~SoundSystemNL()
{
}

bool SoundSystemNL::isAvailable()
{
	return false;
}

void SoundSystemNL::setListenerPos(const Vec3& pos)
{
}

void SoundSystemNL::setListenerAngle(const Vec2& rot)
{
	/* STUB */
}

void SoundSystemNL::setListenerVelocity(const Vec3& vel)
{
	/* STUB */
}

void SoundSystemNL::setMusicVolume(float vol)
{
}

void SoundSystemNL::setSoundVolume(float vol)
{
}

void SoundSystemNL::load(const std::string& soundPath, bool is3D, float minDis)
{
}

void SoundSystemNL::play(const std::string& soundPath)
{
}

void SoundSystemNL::pause(const std::string& soundPath)
{
}

void SoundSystemNL::stop(const std::string& soundPath)
{
}

void SoundSystemNL::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
}

void SoundSystemNL::playMusic(const std::string& soundPath)
{
}

bool SoundSystemNL::isPlayingMusic() const
{
	return false;
}

bool SoundSystemNL::isPlayingMusic(const std::string& soundPath) const
{
	return false;
}

void SoundSystemNL::stopMusic()
{
}

void SoundSystemNL::pauseMusic(bool state)
{
}

void SoundSystemNL::update(float elapsedTime)
{
}

void SoundSystemNL::startEngine()
{
}

void SoundSystemNL::stopEngine()
{
}

void SoundSystemNL::muteAudio()
{
}

void SoundSystemNL::unMuteAudio()
{
}