#ifdef USE_OPENAL
#include "SoundSystemAL.hpp"

#include "common/Utils.hpp"

SoundSystemAL::SoundSystemAL()
{
	_initialized = false;
    _listenerVolume = 1.0;
    _audioMuted = false;
    
    startEngine();
}

SoundSystemAL::~SoundSystemAL()
{
    stopEngine();
}

// Error Checking
#define AL_ERROR_CHECK() AL_ERROR_CHECK_MANUAL(alGetError())
#define AL_ERROR_CHECK_MANUAL(val) \
	{ \
		ALenum __err = val; \
		if (__err != AL_NO_ERROR) \
		{ \
			LOG_E("(%s:%i) OpenAL Error: %s", __FILE__, __LINE__, alGetString(__err)); \
			exit(EXIT_FAILURE); \
		} \
	}

// Delete Sources
void SoundSystemAL::delete_sources()
{
	if (_initialized)
	{
		for (std::vector<ALuint>::iterator source = _sources_idle.begin(); source != _sources_idle.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
		for (std::vector<ALuint>::iterator source = _sources.begin(); source != _sources.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
	}
	_sources_idle.clear();
	_sources.clear();
}

// Delete Buffers
void SoundSystemAL::delete_buffers()
{
	if (_initialized)
	{
		for (std::map<void *, ALuint>::iterator it = _buffers.begin(); it != _buffers.end(); it++)
		//for (auto &it : _buffers)
		{
			if (it->second && alIsBuffer(it->second))
			{
				alDeleteBuffers(1, &it->second);
				AL_ERROR_CHECK();
			}
		}
	}
	_buffers.clear();
}

// Get Buffer
ALuint SoundSystemAL::get_buffer(const SoundDesc& sound)
{
	if (_buffers.count(sound.m_pData) > 0)
	{
		return _buffers[sound.m_pData];
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
		_buffers[sound.m_pData] = buffer;
		return buffer;
	}
}

bool SoundSystemAL::isAvailable()
{
	return _initialized;
}

void SoundSystemAL::setListenerPos(float x, float y, float z)
{
	// Update Listener Position
	alListener3f(AL_POSITION, x, y, z);
	AL_ERROR_CHECK();
	_lastListenerPos = Vec3(x, y, z);
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
	if (!_initialized)
	{
		return;
	}

	// Update Listener Volume
	alListenerf(AL_GAIN, _listenerVolume);
	AL_ERROR_CHECK();

	// Clear Finished Sources
	std::vector<ALuint>::iterator it = _sources.begin();
	while (it != _sources.end())
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
				if (_sources_idle.size() < MAX_IDLE_SOURCES)
				{
					_sources_idle.push_back(source);
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
			it = _sources.erase(it);
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
	if (!_initialized)
	{
		return;
	}

	if (_audioMuted || volume <= 0.0f)
		return;

	bool bIsGUI = AL_FALSE;
	float distance = 0.0f;
	if (x == 0 && y == 0 && z == 0)
	{
		bIsGUI = AL_TRUE;
	}
	else
	{
		distance = Vec3(x, y, z).distanceTo(_lastListenerPos);
		if (distance >= MAX_DISTANCE)
			return;
	}

	// Load Sound
	ALuint buffer = get_buffer(sound);
	if (!buffer)
		return;
	
	// Get Source
	ALuint al_source;
	if (_sources_idle.size() > 0)
	{
		// Use Idle Source
		al_source = _sources_idle.back();
		_sources_idle.pop_back();
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
    // There is a problem with Apple's OpenAL implementation on older Mac OS X versions
    // https://stackoverflow.com/questions/6960731/openal-problem-changing-gain-of-source
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
	_sources.push_back(al_source);
}

void SoundSystemAL::startEngine()
{
    if (_initialized) return;
    
    _device = alcOpenDevice(NULL);
	if (!_device)
	{
		LOG_E("Unable To Load Audio Engine");
		return;
	}
    
	// Create Context
	_context = alcCreateContext(_device, NULL);
	ALCenum err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Open Audio Context: %s", alcGetString(_device, err));
		return;
	}
    
	// Select Context
	alcMakeContextCurrent(_context);
	err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Select Audio Context: %s", alcGetString(_device, err));
		return;
	}
    
	// Set Distance Model
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    
	// Mark As loaded
	_initialized = true;
}

void SoundSystemAL::stopEngine()
{
    if (!_initialized) return;
    
	// Delete Audio Sources
	delete_sources();
    
	// Delete Audio Buffers
	delete_buffers();
    
	// Deselect Context
	alcMakeContextCurrent(NULL);
	ALCenum err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Deselect Audio Context: %s", alcGetString(_device, err));
	}
    
	// Destroy Context
	alcDestroyContext(_context);
	err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Destroy Audio Context: %s", alcGetString(_device, err));
	}
    
	// Close Device
	alcCloseDevice(_device);
	// Can't check for error because _device is closed
	/*err = alcGetError(_device);
     if (err != ALC_NO_ERROR)
     {
     LOG_E("Unable To Close Audio Device: %s", alcGetString(_device, err));
     }*/
    
    // Mark as unloaded
    _initialized = false;
}

void SoundSystemAL::muteAudio()
{
    _audioMuted = true;
}

void SoundSystemAL::unMuteAudio()
{
    _audioMuted = false;
}

#endif
