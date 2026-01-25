#include <assert.h>

#include "SoundStreamOAL.hpp"
#include "common/Logger.hpp"

SoundStreamOAL::SoundStreamOAL()
{
    _createSource();
    _createBuffers();
    m_alFormat = AL_NONE;

    _setVolume(getVolume());
}

SoundStreamOAL::~SoundStreamOAL()
{
    _deleteSource();
    _deleteBuffers();
}

void SoundStreamOAL::_deleteSource()
{
    alDeleteSources(1, &m_source);
    AL_ERROR_CHECK();
}

void SoundStreamOAL::_createSource()
{
    alGenSources(1, &m_source);
}

void SoundStreamOAL::_resetSource()
{
    alSourceStop(m_source);
    AL_ERROR_CHECK();
    alSourceRewind(m_source);
    AL_ERROR_CHECK();
    // Detach all of the buffers from the source
    alSourcei(m_source, AL_BUFFER, AL_NONE);
    AL_ERROR_CHECK();
}

void SoundStreamOAL::_deleteBuffers()
{
    alDeleteBuffers(m_buffers.size(), _getBuffersArray());
    AL_ERROR_CHECK();

    m_buffers.clear();
    m_bufferIdMap.clear();
}

void SoundStreamOAL::_createBuffers()
{
    // Should not be called while buffers are already present
    assert(m_buffers.size() == 0);

    ALuint buffers[2];
    alGenBuffers(2, buffers);
    for (size_t i = 0; i < sizeof(buffers) / sizeof(ALuint); i++)
    {
        m_buffers.push_back(buffers[i]);
        m_bufferIdMap.insert(std::make_pair(buffers[i], i ));
    }
}

void SoundStreamOAL::_resetBuffers()
{
    _deleteBuffers();
    _createBuffers();
}

ALuint* SoundStreamOAL::_getBuffersArray()
{
	ALuint* data;
#ifdef USE_OLD_CPP
	// @NOTE: This has a chance of crashing and burning on older C++ versions, since vectors
	// were not always guaranteed to store their data in a contiguous manner.
	data = &m_buffers[0];
#else
	data = m_buffers.data();
#endif
	
	return data;
}

void SoundStreamOAL::_setVolume(float vol)
{
    alSourcef(m_source, AL_GAIN, vol);
    AL_ERROR_CHECK();
}

void SoundStreamOAL::_play()
{
    alSourcePlay(m_source);
    AL_ERROR_CHECK();
}

void SoundStreamOAL::_pause()
{
    alSourcePause(m_source);
    AL_ERROR_CHECK();
}

bool SoundStreamOAL::_open(const std::string& fileName)
{
    if (m_info.channels == 2) m_alFormat = AL_FORMAT_STEREO16;
    else m_alFormat = AL_FORMAT_MONO16;

    size_t size = m_buffers.size();
    for (size_t i = 0; i < size; i++)
    {
        if (!_stream(i)) return false;
    }
    alSourceQueueBuffers(m_source, size, _getBuffersArray());
    AL_ERROR_CHECK();
    _play();

    return true;
}

void SoundStreamOAL::_close()
{
    _resetSource();
    //_resetBuffers();
    m_alFormat = AL_NONE;
}

void SoundStreamOAL::_update()
{
    ALint processed = 0;

    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
    AL_ERROR_CHECK();

    while (processed--)
    {
        ALuint buffer = 0;

        alSourceUnqueueBuffers(m_source, 1, &buffer);
        AL_ERROR_CHECK();

        int bufferId = m_bufferIdMap[buffer];

        if (!_stream(bufferId))
        {
            bool shouldExit = true;

            if (shouldLoop())
            {
                stb_vorbis_seek_start(m_decoder);
                m_totalSamplesLeft = stb_vorbis_stream_length_in_samples(m_decoder) * m_info.channels;
                shouldExit = !_stream(bufferId);
            }

            if (shouldExit)
            {
                close();
                return;
            }
        }
        alSourceQueueBuffers(m_source, 1, &buffer);
        AL_ERROR_CHECK();
    }
}

void SoundStreamOAL::_publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer)
{
    alBufferData(m_buffers[destBufferId], m_alFormat, sourceBuffer.m_pData, sourceBuffer.m_dataSize * sizeof(int16_t), m_info.sample_rate);
    AL_ERROR_CHECK();
}
