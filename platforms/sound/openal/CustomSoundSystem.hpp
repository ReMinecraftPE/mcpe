#pragma once

#include "thirdparty/OpenAL.h"

#include <vector>
#include <map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3.hpp"

#include "SoundStreamAL.hpp"

//#define SS_AL_SOURCES 12 // 0.10.0

#define SOUND_SYSTEM SoundSystemAL

class SoundSystemAL : public SoundSystem
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	virtual bool isAvailable() override;
	virtual void setListenerPos(const Vec3& pos) override;
	virtual void setListenerAngle(const Vec2& rot) override;

	virtual void setMusicVolume(float vol) override;

	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;

	virtual void playMusic(const std::string& soundPath) override;
	virtual bool isPlayingMusic() const override;
	virtual void stopMusic() override;
	virtual void pauseMusic(bool state) override;
    
	virtual void update(float elapsedTime) override;

    virtual void startEngine() override;
    virtual void stopEngine() override;
    
private:
	bool _hasMaxSources() const;
	ALuint _getIdleSource();
	ALuint _getSource(bool& isNew, bool tryClean = true);
	void _deleteSources();
	void _cleanSources();
	ALuint _getBuffer(const SoundDesc& sound);
	void _deleteBuffers();
	ALenum _getSoundFormat(const PCMSoundHeader& header) const;

	ALCdevice *_device;
	ALCcontext *_context;

	//ALuint m_sources[SS_AL_SOURCES]; // 0.10.0
	std::vector<ALuint> _sources;
	std::vector<ALuint> _sources_idle;
	std::map<void*, ALuint> _buffers;
	SoundStreamAL* _musicStream;

	bool _initialized;

	float _mainVolume;

	Vec3 _listenerPos;
	float _listenerYaw;
};
