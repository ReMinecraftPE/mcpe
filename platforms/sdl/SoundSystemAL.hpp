#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "client/sound/SoundData.hpp"

#define MAX_IDLE_SOURCES 50

class SoundSystemAL
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	void update(float x, float y, float z, float yaw);
	void play(const SoundDesc& sound, float x, float y, float z, float volume, float pitch, bool is_ui);
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
};
