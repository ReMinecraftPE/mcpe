#include "SoundStreamAL.hpp"
#include <assert.h>
#include "common/Logger.hpp"

SoundStreamAL::SoundStreamAL()
{
    _decoder = nullptr;
    _info = stb_vorbis_info();
    _createSource();
    _createBuffers();
    _tempPcmBufferSize = 4096 * 8;
    _tempPcmBuffer = new int16_t[_tempPcmBufferSize];
    _alFormat = AL_NONE;
    _totalSamplesLeft = 0;
    _isStreaming = false;
    _isPaused = false;
    _shouldLoop = false;

    setVolume(1.0f);
}

SoundStreamAL::~SoundStreamAL()
{
    _deleteSource();
    _deleteBuffers();
    _deleteDecoder();
    delete[] _tempPcmBuffer;
}

bool SoundStreamAL::_stream(uint32_t buffer)
{
    int size = 0;
    int result = 0;

    while (size < _tempPcmBufferSize)
    {
        result = stb_vorbis_get_samples_short_interleaved(_decoder, _info.channels, _tempPcmBuffer + size, _tempPcmBufferSize - size);
        if (result > 0) size += result * _info.channels;
        else break;
    }

    if (size == 0) return false;

    alBufferData(buffer, _alFormat, _tempPcmBuffer, size * sizeof(int16_t), _info.sample_rate);
    AL_ERROR_CHECK();
    _totalSamplesLeft -= size;

    return true;
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
    alSourcei(_source, AL_BUFFER, 0);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_deleteBuffers()
{
    alDeleteBuffers(2, _buffers);
    AL_ERROR_CHECK();
}

void SoundStreamAL::_createBuffers()
{
    alGenBuffers(2, _buffers);
}

void SoundStreamAL::_resetBuffers()
{
    _deleteBuffers();
    _createBuffers();
}

void SoundStreamAL::_deleteDecoder()
{
    if (_decoder != nullptr)
        stb_vorbis_close(_decoder);
    _decoder = nullptr;
}

bool SoundStreamAL::open(const std::string& fileName)
{
    if (isStreaming())
    {
        close();
    }

    _isPaused = false;

    _decoder = stb_vorbis_open_filename(fileName.c_str(), NULL, NULL);
    if (!_decoder) return false;
    // Get file info
    _info = stb_vorbis_get_info(_decoder);
    if (_info.channels == 2) _alFormat = AL_FORMAT_STEREO16;
    else _alFormat = AL_FORMAT_MONO16;

    if (!_stream(_buffers[0])) return false;
    if (!_stream(_buffers[1])) return false;
    alSourceQueueBuffers(_source, 2, _buffers);
    AL_ERROR_CHECK();
    alSourcePlay(_source);
    AL_ERROR_CHECK();

    _totalSamplesLeft = stb_vorbis_stream_length_in_samples(_decoder) * _info.channels;
    _isStreaming = true;

    return true;
}

void SoundStreamAL::close()
{
    _resetSource();
    //_resetBuffers();
    _deleteDecoder();
    _alFormat = AL_NONE;
    _totalSamplesLeft = 0;
    _isStreaming = false;
}

void SoundStreamAL::update()
{
    if (!isPlaying()) return;

    int32_t processed = 0;

    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
    AL_ERROR_CHECK();

    while (processed--)
    {
        uint32_t buffer = 0;

        alSourceUnqueueBuffers(_source, 1, &buffer);
        AL_ERROR_CHECK();

        if (!_stream(buffer))
        {
            bool shouldExit = true;

            if (_shouldLoop)
            {
                stb_vorbis_seek_start(_decoder);
                _totalSamplesLeft = stb_vorbis_stream_length_in_samples(_decoder) * _info.channels;
                shouldExit = !_stream(buffer);
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

void SoundStreamAL::setVolume(float vol)
{
    alSourcef(_source, AL_GAIN, vol);
    AL_ERROR_CHECK();
}
