#include "CustomSoundSystem.hpp"

#include "thirdparty/OpenAL.h"
#include "common/Logger.hpp"

#include "SoundStreamAL.hpp"

SoundSystemAL::SoundSystemAL()
{
	_device = nullptr;
	_context = nullptr;
	_initialized = false;
	_musicStream = nullptr;
	_mainVolume = 1.0f;
	_listenerPos = Vec3::ZERO;
	_listenerYaw = 0.0f;
    
    startEngine();
}

SoundSystemAL::~SoundSystemAL()
{
    stopEngine();
}

bool SoundSystemAL::_hasMaxSources() const
{
	return _sources.size() + _sources_idle.size() >= SOUND_MAX_SOURCES;
}

ALuint SoundSystemAL::_getIdleSource()
{
	ALuint al_source = AL_NONE;

	if (_sources_idle.size() > 0)
	{
		// Use Idle Source
		al_source = _sources_idle.back();
		_sources_idle.pop_back();
	}

	return al_source;
}

ALuint SoundSystemAL::_getSource(bool& isNew, bool tryClean)
{
	isNew = false;
	ALuint al_source = _getIdleSource(); // Try to fetch pre-existing idle source
	if (!al_source)
	{
		// Could not find pre-existing idle source

		if (_hasMaxSources())
		{
			if (tryClean)
			{
				// Clean finished sources since no idle ones are available
				_cleanSources();

				// Did some cleaning, lets try again...
				return _getSource(isNew, false);
			}
			else
			{
				// Too many sources already exist and they're all in-use, sucks to suck...
				return AL_NONE;
			}
		}

		// Create Source
		alGenSources(1, &al_source);
		// Special Out-Of-Memory Handling
		{
			ALenum err = alGetError();
			if (err == AL_OUT_OF_MEMORY)
			{
				return AL_NONE;
			}
			else
			{
				AL_ERROR_CHECK_MANUAL(err);
			}
		}
		isNew = true;
	}

	return al_source;
}

// Delete Sources
void SoundSystemAL::_deleteSources()
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

// Clear Finished Sources
void SoundSystemAL::_cleanSources()
{
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
				_sources_idle.push_back(source);

				// Reset playback state of source to prevent buffer ghosting on legacy Mac OS X and Windows.
				// see: https://stackoverflow.com/questions/6960731/openal-problem-changing-gain-of-source
				alSourceStop(source);
				AL_ERROR_CHECK();
				alSourceRewind(source);
				AL_ERROR_CHECK();
			}
		}
		else
		{
			// Not A Source
			remove = true;
		}
		// Remove If Needed
		if (remove)
		{
			it = _sources.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// Get Buffer
ALuint SoundSystemAL::_getBuffer(const SoundDesc& sound)
{
	// Fetch pre-existing buffer
	if (_buffers.count(sound.m_buffer.m_pData) > 0)
	{
		return _buffers[sound.m_buffer.m_pData];
	}
	
	// Create Buffer
	ALuint buffer;
	alGenBuffers(1, &buffer);
	AL_ERROR_CHECK();
	alBufferData(buffer, _getSoundFormat(sound.m_header), sound.m_buffer.m_pData, sound.m_buffer.m_dataSize, sound.m_header.m_sample_rate);
	AL_ERROR_CHECK();

	// Store
	_buffers[sound.m_buffer.m_pData] = buffer;
	return buffer;
}

// Delete Buffers
void SoundSystemAL::_deleteBuffers()
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

ALenum SoundSystemAL::_getSoundFormat(const PCMSoundHeader& header) const
{
	switch (header.m_channels)
	{
	case 1:
		return header.m_bytes_per_sample == 2 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
	case 2:
		return header.m_bytes_per_sample == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
	default:
		return AL_NONE;
	}
}

bool SoundSystemAL::isAvailable()
{
	return _initialized;
}

void SoundSystemAL::setListenerPos(const Vec3& pos)
{
	// Empty on iOS 0.10.0
	if (_listenerPos == pos)
		return; // No need to waste time doing math and talking to OpenAL

	// Update Listener Position
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	AL_ERROR_CHECK();
	_listenerPos = pos;
	
	// Update Listener Volume
	alListenerf(AL_GAIN, _mainVolume);
	AL_ERROR_CHECK();
}

void SoundSystemAL::setListenerAngle(const Vec2& rot)
{
	if (_listenerYaw == rot.x)
		return; // No need to waste time doing math and talking to OpenAL

	// Update Listener Orientation
	float radian_yaw = rot.x * (M_PI / 180);
	ALfloat orientation[] = { -sinf(radian_yaw), 0.0f, cosf(radian_yaw), 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	AL_ERROR_CHECK();
	_listenerYaw = rot.x;
}

void SoundSystemAL::setMusicVolume(float vol)
{
	assert(_musicStream != nullptr);

	_musicStream->setVolume(vol);
}

void SoundSystemAL::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
	// Check
	if (!_initialized)
	{
		return;
	}

	if (volume <= 0.0f)
		return;

	bool bIsGUI = AL_FALSE;
	float distance = 0.0f;
	if (pos == Vec3::ZERO)
	{
		bIsGUI = AL_TRUE;
	}

	// Load Sound
	ALuint buffer = _getBuffer(sound);
	if (!buffer)
		return;

	// Get Source
	bool isNew;
	ALuint al_source = _getSource(isNew);
	if (!al_source)
	{
		// Couldn't get a source, just give up.
		return;
	}

	// Set Properties
	alSourcef(al_source, AL_PITCH, pitch);
	AL_ERROR_CHECK();
	alSourcef(al_source, AL_GAIN, volume);
	AL_ERROR_CHECK();
	alSource3f(al_source, AL_POSITION, pos.x, pos.y, pos.z);
	AL_ERROR_CHECK();
	alSourcei(al_source, AL_SOURCE_RELATIVE, bIsGUI);
	AL_ERROR_CHECK();

	// Only set constant parameters if source isn't reused
	if (isNew)
	{
		// Set Attenuation
		alSourcef(al_source, AL_MAX_DISTANCE, SOUND_MAX_DISTANCE);
		AL_ERROR_CHECK();
		alSourcef(al_source, AL_ROLLOFF_FACTOR, 0.9f); // 0.9f is audibly on-par with b1.2_02's rolloff factor. So you probably shouldn't change it. 0.03f is default value for Paulscode.
		AL_ERROR_CHECK();
		alSourcef(al_source, AL_REFERENCE_DISTANCE, 5.0f); // Sounds the same regardless of being set. Paulscode doesn't set this.
		AL_ERROR_CHECK();

		alSource3f(al_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		AL_ERROR_CHECK();
		alSourcei(al_source, AL_LOOPING, AL_FALSE);
		AL_ERROR_CHECK();
	}
	else
	{
		// Detach all of the buffers from the source
		alSourcei(al_source, AL_BUFFER, AL_NONE);
		AL_ERROR_CHECK();
	}

	// Set Buffer
	alSourcei(al_source, AL_BUFFER, buffer);
	AL_ERROR_CHECK();

	// Play
	alSourcePlay(al_source);
	AL_ERROR_CHECK();
	_sources.push_back(al_source);
}

void SoundSystemAL::playMusic(const std::string& soundPath)
{
	_musicStream->open(soundPath);
}

bool SoundSystemAL::isPlayingMusic() const
{
	return _musicStream->isPlaying();
}

void SoundSystemAL::stopMusic()
{
	_musicStream->close();
}

void SoundSystemAL::pauseMusic(bool state)
{
	_musicStream->setPausedState(state);
}

void SoundSystemAL::update(float elapsedTime)
{
	_musicStream->update();
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
		LOG_E("Unable To Open Audio Context: %ss", alcGetString(_device, err));
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

	// From 0.10.0
	/*alGenSources(SS_AL_SOURCES, m_sources);
	for (int i = 0; i < SS_AL_SOURCES; i++)
	{
		ALuint sid = m_sources[i];
		alSourcef(sid, AL_REFERENCE_DISTANCE, 5.0f);
		alSourcef(sid, AL_MAX_DISTANCE, SS_AL_MAX_DISTANCE);
		alSourcef(sid, AL_ROLLOFF_FACTOR, 6.0f);
	}*/

	// From 0.10.0
	/*const ALfloat position[12] = {};
	alListenerfv(AL_POSITION, position);
	AL_ERROR_CHECK();
	const ALfloat orientation[6] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	AL_ERROR_CHECK();
	const ALfloat velocity[3] = { };
	alListenerfv(AL_VELOCITY, velocity);
	AL_ERROR_CHECK();*/

	_musicStream = new SoundStreamAL();
    
	// Mark As loaded
	_initialized = true;
}

void SoundSystemAL::stopEngine()
{
    if (!_initialized) return;

	delete _musicStream;
    
	// Delete Audio Sources
	_deleteSources();
    
	// Delete Audio Buffers
	_deleteBuffers();
    
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
