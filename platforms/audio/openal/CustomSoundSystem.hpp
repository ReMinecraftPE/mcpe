#pragma once

#include "thirdparty/OpenAL.h"

#include <vector>
#include <map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3.hpp"

#include "SoundStreamOAL.hpp"

//#define SS_OAL_SOURCES 12 // 0.10.0

#define SOUND_SYSTEM SoundSystemOAL

class SoundSystemOAL : public SoundSystem
{
public:
	SoundSystemOAL();
	~SoundSystemOAL();

private:
	bool _hasMaxSources() const;
	ALuint _getIdleSource();
	ALuint _getSource(bool& isNew, bool tryClean = true);
	void _deleteSources();
	void _cleanSources();
	ALuint _getBuffer(const SoundDesc& sound);
	void _deleteBuffers();
	ALenum _getSoundFormat(const PCMSoundHeader& header) const;

public:
	bool isAvailable() override;
	void setListenerPos(const Vec3& pos) override;
	void setListenerAngle(const Vec2& rot) override;

	void setMusicVolume(float vol) override;

	void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;

	void playMusic(const std::string& soundPath) override;
	bool isPlayingMusic() const override;
	void stopMusic() override;
	void pauseMusic(bool state) override;
    
	void update(float elapsedTime) override;

    void startEngine() override;
    void stopEngine() override;

private:
	ALCdevice *_device;
	ALCcontext *_context;

	//ALuint m_sources[SS_AL_SOURCES]; // 0.10.0
	std::vector<ALuint> m_sources;
	std::vector<ALuint> m_sources_idle;
	std::map<void*, ALuint> m_buffers;
	SoundStreamOAL* m_musicStream;

	bool m_bInitialized;

	float m_mainVolume;

	Vec3 m_listenerPos;
	float m_listenerYaw;
};
