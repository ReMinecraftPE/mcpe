#include <assert.h>

#include "CustomSoundSystem.hpp"

#include "thirdparty/OpenAL.h"
#include "common/Logger.hpp"
#include "common/Utils.hpp"

#include "SoundStreamOAL.hpp"

SoundSystemOAL::SoundSystemOAL()
{
	_device = nullptr;
	_context = nullptr;
	m_bInitialized = false;
	m_musicStream = nullptr;
	m_mainVolume = 1.0f;
	m_listenerPos = Vec3::ZERO;
	m_listenerYaw = 0.0f;
    
    startEngine();
}

SoundSystemOAL::~SoundSystemOAL()
{
    stopEngine();
}

bool SoundSystemOAL::_hasMaxSources() const
{
	return m_sources.size() + m_sources_idle.size() >= SOUND_MAX_SOURCES;
}

ALuint SoundSystemOAL::_getIdleSource()
{
	ALuint al_source = AL_NONE;

	if (m_sources_idle.size() > 0)
	{
		// Use Idle Source
		al_source = m_sources_idle.back();
		m_sources_idle.pop_back();
	}

	return al_source;
}

ALuint SoundSystemOAL::_getSource(bool& isNew, bool tryClean)
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
void SoundSystemOAL::_deleteSources()
{
	if (m_bInitialized)
	{
		for (std::vector<ALuint>::iterator source = m_sources_idle.begin(); source != m_sources_idle.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
		for (std::vector<ALuint>::iterator source = m_sources.begin(); source != m_sources.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
	}
	m_sources_idle.clear();
	m_sources.clear();
}

// Clear Finished Sources
void SoundSystemOAL::_cleanSources()
{
	std::vector<ALuint>::iterator it = m_sources.begin();
	while (it != m_sources.end())
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
				m_sources_idle.push_back(source);

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
			it = m_sources.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// Get Buffer
ALuint SoundSystemOAL::_getBuffer(const SoundDesc& sound)
{
	// Fetch pre-existing buffer
	if (m_buffers.count(sound.m_buffer.m_pData) > 0)
	{
		return m_buffers[sound.m_buffer.m_pData];
	}
	
	// Create Buffer
	ALuint buffer;
	alGenBuffers(1, &buffer);
	AL_ERROR_CHECK();
	alBufferData(buffer, _getSoundFormat(sound.m_header), sound.m_buffer.m_pData, sound.m_buffer.m_dataSize, sound.m_header.m_sample_rate);
	AL_ERROR_CHECK();

	// Store
	m_buffers[sound.m_buffer.m_pData] = buffer;
	return buffer;
}

// Delete Buffers
void SoundSystemOAL::_deleteBuffers()
{
	if (m_bInitialized)
	{
		for (std::map<void *, ALuint>::iterator it = m_buffers.begin(); it != m_buffers.end(); it++)
		//for (auto &it : m_buffers)
		{
			if (it->second && alIsBuffer(it->second))
			{
				alDeleteBuffers(1, &it->second);
				AL_ERROR_CHECK();
			}
		}
	}
	m_buffers.clear();
}

ALenum SoundSystemOAL::_getSoundFormat(const PCMSoundHeader& header) const
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

bool SoundSystemOAL::isAvailable()
{
	return m_bInitialized;
}

void SoundSystemOAL::setListenerPos(const Vec3& pos)
{
	// Empty on iOS 0.10.0
	if (m_listenerPos == pos)
		return; // No need to waste time doing math and talking to OpenAL

	// Update Listener Position
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	AL_ERROR_CHECK();
	m_listenerPos = pos;
	
	// Update Listener Volume
	alListenerf(AL_GAIN, m_mainVolume);
	AL_ERROR_CHECK();
}

void SoundSystemOAL::setListenerAngle(const Vec2& rot)
{
	if (m_listenerYaw == rot.x)
		return; // No need to waste time doing math and talking to OpenAL

	// Update Listener Orientation
	float radian_yaw = rot.x * MTH_DEG_TO_RAD;
	Vec3 orientation[] = { Vec3(-Mth::sin(radian_yaw), 0.0f, Mth::cos(radian_yaw)), Vec3::UNIT_Y };
	alListenerfv(AL_ORIENTATION, (const ALfloat*)orientation);
	AL_ERROR_CHECK();
	m_listenerYaw = rot.x;
}

void SoundSystemOAL::setMusicVolume(float vol)
{
	assert(m_musicStream != nullptr);

	m_musicStream->setVolume(vol);
}

void SoundSystemOAL::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
	// Check
	if (!m_bInitialized)
	{
		return;
	}

	if (volume <= 0.0f)
		return;

	bool bIsGUI = AL_FALSE;
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
	m_sources.push_back(al_source);
}

void SoundSystemOAL::playMusic(const std::string& soundPath)
{
	m_musicStream->open(soundPath);
}

bool SoundSystemOAL::isPlayingMusic() const
{
	return m_musicStream->isPlaying();
}

void SoundSystemOAL::stopMusic()
{
	m_musicStream->close();
}

void SoundSystemOAL::pauseMusic(bool state)
{
	m_musicStream->setPausedState(state);
}

void SoundSystemOAL::update(float elapsedTime)
{
	m_musicStream->update();
}

void SoundSystemOAL::startEngine()
{
    if (m_bInitialized) return;
    
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

	m_musicStream = new SoundStreamOAL();
    
	// Mark As loaded
	m_bInitialized = true;
}

void SoundSystemOAL::stopEngine()
{
    if (!m_bInitialized) return;

	delete m_musicStream;
    
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
    m_bInitialized = false;
}
