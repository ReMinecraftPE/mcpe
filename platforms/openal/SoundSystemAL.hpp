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

//#define SS_AL_SOURCES 12 // 0.10.0

class SoundSystemAL : public SoundSystem
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	virtual bool isAvailable();
	void update();
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch);

	virtual void setListenerPos(const Vec3& pos);
	virtual void setListenerAngle(const Vec2& rot);
    
    virtual void startEngine();
    virtual void stopEngine();
    
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
	bool _initialized;
	std::vector<ALuint> _sources;
	std::vector<ALuint> _sources_idle;
	std::map<void *, ALuint> _buffers;

	Vec3 _listenerPos;
	float _listenerYaw;
    float _listenerVolume;
};

#endif
