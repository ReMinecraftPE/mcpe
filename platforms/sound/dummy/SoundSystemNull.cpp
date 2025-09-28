#include "CustomSoundSystem.hpp"

SoundSystemNull::~SoundSystemNull()
{
}

bool SoundSystemNull::isAvailable()
{
	return true;
}

void SoundSystemNull::setListenerPos(const Vec3& pos)
{
}

void SoundSystemNull::setListenerAngle(const Vec2& rot)
{
	/* STUB */
}

void SoundSystemNull::setListenerVelocity(const Vec3& vel)
{
	/* STUB */
}

void SoundSystemNull::setMusicVolume(float vol)
{
}

void SoundSystemNull::setSoundVolume(float vol)
{
}

void SoundSystemNull::load(const std::string& soundPath, bool is3D, float minDis)
{
}

void SoundSystemNull::play(const std::string& soundPath)
{
}

void SoundSystemNull::pause(const std::string& soundPath)
{
}

void SoundSystemNull::stop(const std::string& soundPath)
{
}

void SoundSystemNull::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
}

void SoundSystemNull::playMusic(const std::string& soundPath)
{
}

bool SoundSystemNull::isPlayingMusic() const
{
	return false;
}

bool SoundSystemNull::isPlayingMusic(const std::string& soundPath) const
{
	return false;
}

void SoundSystemNull::stopMusic()
{
}

void SoundSystemNull::pauseMusic(bool state)
{
}

void SoundSystemNull::update(float elapsedTime)
{
}

void SoundSystemNull::startEngine()
{
}

void SoundSystemNull::stopEngine()
{
}

void SoundSystemNull::muteAudio()
{
}

void SoundSystemNull::unMuteAudio()
{
}