#pragma once

#include <string>
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"

#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"

#define SOUND_SYSTEM SoundSystemNull

class SoundSystemNull : public SoundSystem
{
public:
	~SoundSystemNull();

	bool isAvailable() override;
	void setListenerPos(const Vec3& pos) override;
	void setListenerAngle(const Vec2& rot) override;
	void setListenerVelocity(const Vec3& vel) override;

	void setMusicVolume(float vol) override;
	void setSoundVolume(float vol) override;

	void load(const std::string& soundPath, bool is3D, float minDis) override;
	void play(const std::string& soundPath) override;
	void pause(const std::string& soundPath) override;
	void stop(const std::string& soundPath) override;
	void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;

	void playMusic(const std::string& soundPath) override;
	bool isPlayingMusic() const override;
	bool isPlayingMusic(const std::string& soundPath) const override;
	void stopMusic() override;
	void pauseMusic(bool state) override;

	void update(float elapsedTime) override;
    
    void startEngine() override;
    void stopEngine() override;
    
    void muteAudio() override;
    void unMuteAudio() override;
};

