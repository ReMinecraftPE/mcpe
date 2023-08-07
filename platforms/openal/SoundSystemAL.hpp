#pragma once

#ifdef _WIN32
#include <thirdparty/OpenAL/Include/al.h>
#include <thirdparty/OpenAL/Include/alc.h>
#pragma comment( lib, "OpenAl32.lib" )
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <string>
#include <vector>
#include <unordered_map>

#include "client/sound/SoundSystem.hpp"
#include <client/common/Vec3.hpp>

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
private:
	void delete_sources();
	void delete_buffers();
	ALuint get_buffer(const SoundDesc& sound);

	ALCdevice *device = NULL;
	ALCcontext *context = NULL;
	bool loaded = false;
	std::vector<ALuint> sources;
	std::vector<ALuint> idle_sources;
	std::unordered_map<void *, ALuint> buffers;

	Vec3 lastListenerPos;
};
