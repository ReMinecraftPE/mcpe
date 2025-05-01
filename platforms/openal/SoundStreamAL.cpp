#include "SoundStreamAL.hpp"
#include <assert.h>

SoundStreamAL::SoundStreamAL()
{
    _decoder = nullptr;
    _info = stb_vorbis_info();
    alGenSources(1, &_source);
    alGenBuffers(2, _buffers);
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
    alDeleteSources(1, &_source);
    alDeleteBuffers(2, _buffers);
    close();
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
    _totalSamplesLeft -= size;

    return true;
}

bool SoundStreamAL::open(const std::string& fileName)
{
    if (isStreaming())
    {
        close();
    }

    _decoder = stb_vorbis_open_filename(fileName.c_str(), NULL, NULL);
    if (!_decoder) return false;
    // Get file info
    _info = stb_vorbis_get_info(_decoder);
    if (_info.channels == 2) _alFormat = AL_FORMAT_STEREO16;
    else _alFormat = AL_FORMAT_MONO16;

    if (!_stream(_buffers[0])) return false;
    if (!_stream(_buffers[1])) return false;
    alSourceQueueBuffers(_source, 2, _buffers);
    alSourcePlay(_source);

    _totalSamplesLeft = stb_vorbis_stream_length_in_samples(_decoder) * _info.channels;
    _isStreaming = true;

    return true;
}

void SoundStreamAL::close()
{
    alSourceStop(_source);
    alSourceRewind(_source);
    if (_decoder != nullptr)
        stb_vorbis_close(_decoder);
    _decoder = nullptr;
    _alFormat = AL_NONE;
    _totalSamplesLeft = 0;
    _isStreaming = false;
}

void SoundStreamAL::update()
{
    if (!isPlaying()) return;

    int32_t processed = 0;

    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);

    while (processed--)
    {
        uint32_t buffer = 0;

        alSourceUnqueueBuffers(_source, 1, &buffer);

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
    }
}

void SoundStreamAL::setVolume(float vol)
{
    alSourcef(_source, AL_GAIN, vol);
    assert(alGetError() == AL_NO_ERROR);
}
