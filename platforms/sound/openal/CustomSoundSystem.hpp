#pragma once


#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(__EMSCRIPTEN__)
#include <AL/al.h>
#include <AL/alc.h>
#else
#include "al.h"
#include "alc.h"
#ifdef _WIN32
#pragma comment( lib, "OpenAL32.lib" )
#endif
#endif

#include <string>
#include <vector>
#include <map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3.hpp"

#define MAX_IDLE_SOURCES 50
#define MAX_DISTANCE 16.0f

#define SOUND_SYSTEM SoundSystemAL

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
