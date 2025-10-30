#include <assert.h>

#include "SoundStreamAL.hpp"
#include "common/Logger.hpp"

SoundStreamAL::SoundStreamAL()
    : SoundStream()
{
    _createSource();
    _createBuffers();
    _alFormat = AL_NONE;

    _setVolume(getVolume());
}

SoundStreamAL::~SoundStreamAL()
{
    _deleteSource();
    _deleteBuffers();
}

void SoundStreamAL::_deleteSource()
{
    alDeleteSources(1, &_source);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_createSource()
{
    alGenSources(1, &_source);
}

void SoundStreamAL::_resetSource()
{
    alSourceStop(_source);
    AL_ERROR_CHECK();
    alSourceRewind(_source);
    AL_ERROR_CHECK();
    // Detach all of the buffers from the source
    alSourcei(_source, AL_BUFFER, AL_NONE);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_deleteBuffers()
{
    alDeleteBuffers(_buffers.size(), _getBuffersArray());
    AL_ERROR_CHECK();

    _buffers.clear();
    _bufferIdMap.clear();
}

void SoundStreamAL::_createBuffers()
{
    // Should not be called while buffers are already present
    assert(_buffers.size() == 0);

    ALuint buffers[2];
    alGenBuffers(2, buffers);
    for (int i = 0; i < sizeof(buffers) / sizeof(ALuint); i++)
    {
        _buffers.push_back(buffers[i]);
        _bufferIdMap.insert(std::make_pair(buffers[i], i ));
    }
}

void SoundStreamAL::_resetBuffers()
{
    _deleteBuffers();
    _createBuffers();
}

ALuint* SoundStreamAL::_getBuffersArray()
{
	ALuint* data;
#ifdef USE_OLD_CPP
	// @NOTE: This has a chance of crashing and burning on older C++ versions, since vectors
	// were not always guaranteed to store their data in a contiguous manner.
	data = &_buffers[0];
#else
	data = _buffers.data();
#endif
	
	return data;
}

void SoundStreamAL::_setVolume(float vol)
{
    alSourcef(_source, AL_GAIN, vol);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_play()
{
    alSourcePlay(_source);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_pause()
{
    alSourcePause(_source);
    AL_ERROR_CHECK();
}

bool SoundStreamAL::_open(const std::string& fileName)
{
    if (m_info.channels == 2) _alFormat = AL_FORMAT_STEREO16;
    else _alFormat = AL_FORMAT_MONO16;

    size_t size = _buffers.size();
    for (int i = 0; i < size; i++)
    {
        if (!_stream(i)) return false;
    }
    alSourceQueueBuffers(_source, size, _getBuffersArray());
    AL_ERROR_CHECK();
    _play();

    return true;
}

void SoundStreamAL::_close()
{
    _resetSource();
    //_resetBuffers();
    _alFormat = AL_NONE;
}

void SoundStreamAL::_update()
{
    ALint processed = 0;

    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
    AL_ERROR_CHECK();

    while (processed--)
    {
        ALuint buffer = 0;

        alSourceUnqueueBuffers(_source, 1, &buffer);
        AL_ERROR_CHECK();

        int bufferId = _bufferIdMap[buffer];

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
        alSourceQueueBuffers(_source, 1, &buffer);
        AL_ERROR_CHECK();
    }
}

void SoundStreamAL::_publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer)
{
    alBufferData(_buffers[destBufferId], _alFormat, sourceBuffer.m_pData, sourceBuffer.m_dataSize * sizeof(int16_t), m_info.sample_rate);
    AL_ERROR_CHECK();
}