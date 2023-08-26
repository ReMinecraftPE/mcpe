#ifdef USE_OPENAL
#include "SoundSystemAL.hpp"

#include "common/Utils.hpp"

SoundSystemAL::SoundSystemAL()
{
	loaded = false;
	
	device = alcOpenDevice(NULL);
	if (!device)
	{
		LogMsg("Unable To Load Audio Engine");
		return;
	}

	// Create Context
	context = alcCreateContext(device, NULL);
	ALCenum err = alcGetError(device);
	if (err != ALC_NO_ERROR)
	{
		LogMsg("Unable To Open Audio Context: %s", alcGetString(device, err));
		return;
	}

	// Select Context
	alcMakeContextCurrent(context);
	err = alcGetError(device);
	if (err != ALC_NO_ERROR)
	{
		LogMsg("Unable To Select Audio Context: %s", alcGetString(device, err));
		return;
	}

	// Set Distance Model
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	// Mark As Loaded
	loaded = true;
}

SoundSystemAL::~SoundSystemAL()
{
	if (!loaded)
	{
		return;
	}

	// Delete Audio Sources
	delete_sources();

	// Delete Audio Buffers
	delete_buffers();

	// Deselect Context
	alcMakeContextCurrent(NULL);
	ALCenum err = alcGetError(device);
	if (err != ALC_NO_ERROR)
	{
		LogMsg("Unable To Deselect Audio Context: %s", alcGetString(device, err));
	}

	// Destroy Context
	alcDestroyContext(context);
	err = alcGetError(device);
	if (err != ALC_NO_ERROR)
	{
		LogMsg("Unable To Destroy Audio Context: %s", alcGetString(device, err));
	}

	// Close Device
	alcCloseDevice(device);
	// Can't check for error because device is closed
	/*err = alcGetError(device);
	if (err != ALC_NO_ERROR)
	{
		LogMsg("Unable To Close Audio Device: %s", alcGetString(device, err));
	}*/
}

// Error Checking
#define AL_ERROR_CHECK() AL_ERROR_CHECK_MANUAL(alGetError())
#define AL_ERROR_CHECK_MANUAL(val) \
	{ \
		ALenum __err = val; \
		if (__err != AL_NO_ERROR) \
		{ \
			LogMsg("(%s:%i) OpenAL Error: %s", __FILE__, __LINE__, alGetString(__err)); \
			exit(EXIT_FAILURE); \
		} \
	}

// Delete Sources
void SoundSystemAL::delete_sources()
{
	if (loaded)
	{
		for (std::vector<ALuint>::iterator source = idle_sources.begin(); source != idle_sources.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
		for (std::vector<ALuint>::iterator source = sources.begin(); source != sources.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
	}
	idle_sources.clear();
	sources.clear();
}

// Delete Buffers
void SoundSystemAL::delete_buffers()
{
	if (loaded)
	{
		for (std::map<void *, ALuint>::iterator it = buffers.begin(); it != buffers.end(); it++)
		//for (auto &it : buffers)
		{
			if (it->second && alIsBuffer(it->second))
			{
				alDeleteBuffers(1, &it->second);
				AL_ERROR_CHECK();
			}
		}
	}
	buffers.clear();
}

// Get Buffer
ALuint SoundSystemAL::get_buffer(const SoundDesc& sound)
{
	if (buffers.count(sound.m_pData) > 0)
	{
		return buffers[sound.m_pData];
	}
	else
	{
		// Sound Format
		ALenum format = AL_NONE;
		if (sound.m_header.m_channels == 1)
		{
			format = sound.m_header.m_bytes_per_sample == 2 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
		}
		else if (sound.m_header.m_channels == 2)
		{
			format = sound.m_header.m_bytes_per_sample == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
		}

		// Sound Data Size
		int size = sound.m_header.m_channels * sound.m_header.m_length * sound.m_header.m_bytes_per_sample;

		// Create Buffer
		ALuint buffer;
		alGenBuffers(1, &buffer);
		AL_ERROR_CHECK();
		alBufferData(buffer, format, sound.m_pData, size, sound.m_header.m_sample_rate);
		AL_ERROR_CHECK();

		// Store
		buffers[sound.m_pData] = buffer;
		return buffer;
	}
}

bool SoundSystemAL::isAvailable()
{
	return loaded;
}

void SoundSystemAL::setListenerPos(float x, float y, float z)
{
	// Update Listener Position
	alListener3f(AL_POSITION, x, y, z);
	AL_ERROR_CHECK();
	lastListenerPos = Vec3(x, y, z);
	update();
}

void SoundSystemAL::setListenerAngle(float yaw, float pitch)
{
	// Update Listener Orientation
	float radian_yaw = yaw * (M_PI / 180);
	ALfloat orientation[] = { -sinf(radian_yaw), 0.0f, cosf(radian_yaw), 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	AL_ERROR_CHECK();
}

void SoundSystemAL::update()
{
	// Check
	if (!loaded)
	{
		return;
	}

	// Update Listener Volume
	float volume = 1;
	alListenerf(AL_GAIN, volume);
	AL_ERROR_CHECK();

	// Clear Finished Sources
	std::vector<ALuint>::iterator it = sources.begin();
	while (it != sources.end())
	{
		ALuint source = *it;
		bool remove = false;
		// Check
		if (source && alIsSource(source))
		{
			// Is Valid Source
			ALint source_state;
			alGetSourcei(source, AL_SOURCE_STATE, &source_state);
			AL_ERROR_CHECK();
			if (source_state != AL_PLAYING)
			{
				// Finished Playing
				remove = true;
				if (idle_sources.size() < MAX_IDLE_SOURCES)
				{
					idle_sources.push_back(source);
				}
				else
				{
					alDeleteSources(1, &source);
					AL_ERROR_CHECK();
				}
			}
		}
		else
		{
			// Not A Source
			remove = true;
		}
		// Remove If Needed
		if (remove) {
			it = sources.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void SoundSystemAL::playAt(const SoundDesc& sound, float x, float y, float z, float volume, float pitch)
{
	// Check
	if (!loaded)
	{
		return;
	}

	if (volume <= 0.0f)
		return;

	bool bIsGUI = AL_FALSE;
	float distance = 0.0f;
	if (x == 0 && y == 0 && z == 0)
	{
		bIsGUI = AL_TRUE;
	}
	else
	{
		distance = Vec3(x, y, z).distanceTo(lastListenerPos);
		if (distance >= MAX_DISTANCE)
			return;
	}

	// Load Sound
	ALuint buffer = get_buffer(sound);
	if (!buffer)
		return;
	
	// Get Source
	ALuint al_source;
	if (idle_sources.size() > 0)
	{
		// Use Idle Source
		al_source = idle_sources.back();
		idle_sources.pop_back();
	}
	else
	{
		// Create Source
		alGenSources(1, &al_source);
		// Special Out-Of-Memory Handling
		{
			ALenum err = alGetError();
			if (err == AL_OUT_OF_MEMORY)
			{
				return;
			}
			else
			{
				AL_ERROR_CHECK_MANUAL(err);
			}
		}
	}

	// Set Properties
	alSourcef(al_source, AL_PITCH, pitch);
	AL_ERROR_CHECK();
	alSourcef(al_source, AL_GAIN, volume);
	AL_ERROR_CHECK();
	alSource3f(al_source, AL_POSITION, x, y, z);
	AL_ERROR_CHECK();
	alSource3f(al_source, AL_VELOCITY, 0, 0, 0);
	AL_ERROR_CHECK();
	alSourcei(al_source, AL_LOOPING, AL_FALSE);
	AL_ERROR_CHECK();
	alSourcei(al_source, AL_SOURCE_RELATIVE, bIsGUI);
	AL_ERROR_CHECK();

	// Set Attenuation
	alSourcef(al_source, AL_MAX_DISTANCE, MAX_DISTANCE);
	AL_ERROR_CHECK();
	alSourcef(al_source, AL_ROLLOFF_FACTOR, 1.0f);
	AL_ERROR_CHECK();
	alSourcef(al_source, AL_REFERENCE_DISTANCE, 5.0f);
	AL_ERROR_CHECK();

	// Set Buffer
	alSourcei(al_source, AL_BUFFER, buffer);
	AL_ERROR_CHECK();

	// Play
	alSourcePlay(al_source);
	AL_ERROR_CHECK();
	sources.push_back(al_source);
}

#endif
