#pragma once

#ifdef USE_OPENAL

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#pragma comment( lib, "OpenAl32.lib" )
#elif defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <string>
#include <vector>
#include <map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3.hpp"

#define MAX_IDLE_SOURCES 50
#define MAX_DISTANCE 16.0f

class SoundSystemAL : public SoundSystem
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	virtual bool isAvailable();
	void update();
	virtual void playAt(const SoundDesc& sound, float x, float y, float z, float volume, float pitch);

	virtual void setListenerPos(float x, float y, float z);
	virtual void setListenerAngle(float yaw, float pitch);
    
    virtual void startEngine();
    virtual void stopEngine();
    virtual void muteAudio();
    virtual void unMuteAudio();
    
private:
	void delete_sources();
	void delete_buffers();
	ALuint get_buffer(const SoundDesc& sound);

	ALCdevice *_device;
	ALCcontext *_context;
	bool _initialized;
	std::vector<ALuint> _sources;
	std::vector<ALuint> _sources_idle;
	std::map<void *, ALuint> _buffers;

	Vec3 _lastListenerPos;
    float _listenerVolume;
    bool _audioMuted;
};

#endif
