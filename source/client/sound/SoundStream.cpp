#include "SoundStream.hpp"

#if 0
SoundStream::SoundStream()
{
    alGenSources(1, &_source);
    alGenBuffers(2, _buffers);
    _bufferSize = 4096 * 8;
    _shouldLoop = false;
}

SoundStream::~SoundStream()
{
    alDeleteSources(1, & _source);
    alDeleteBuffers(2, _buffers);
    stb_vorbis_close(_stream);
}

bool SoundStream::stream(uint32_t buffer)
{
    // Uncomment this to avoid VLAs
    //#define BUFFER_SIZE 4096*32
    #ifndef BUFFER_SIZE // VLAs ftw
    #define BUFFER_SIZE (_bufferSize)
    #endif
    int16_t pcm[BUFFER_SIZE];
    int size = 0;
    int sresult = 0;

    while (size < BUFFER_SIZE)
    {
        result = stb_vorbis_get_samples_short_interleaved(_stream, _info.channels, pcm+size, BUFFER_SIZE-size);
        if (result > 0) size += result*_info.channels;
        else break;
    }

    if (size == 0) return false;

    /// 1 API-specific function ///
    alBufferData(buffer, _format, pcm, size*sizeof(int16_t), _info.sample_rate);
    _totalSamplesLeft -= size;
    #undef BUFFER_SIZE

    return true;
}

bool SoundStream::open(const char* filename)
{
    _stream = stb_vorbis_open_filename((char*)filename, NULL, NULL);
    if (!_stream) return false;
    // Get file info
    _info = stb_vorbis_get_info(_stream);
    if(_info.channels == 2) _format = AL_FORMAT_STEREO16;
    else _format = AL_FORMAT_MONO16;

    if(!stream(_buffers[0])) return false;
    if(!stream(_buffers[1])) return false;
    /// 2 API-specific functions ///
    alSourceQueueBuffers(_source, 2, _buffers);
    alSourcePlay(_source);

    _totalSamplesLeft = stb_vorbis_stream_length_in_samples(_stream) * _info.channels;

    return true;
}

bool SoundStream::update()
{
    int32_t processed = 0;

    /// 1 API-specific function ///
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);

    while (processed--)
    {
        uint32_t buffer = 0;
        
        /// 1 API-specific function ///
        alSourceUnqueueBuffers(_source, 1, &buffer);

        if (!stream(buffer))
        {
            bool shouldExit = true;

            if (_shouldLoop)
            {
                stb_vorbis_seek_start(_stream);
                _totalSamplesLeft = stb_vorbis_stream_length_in_samples(_stream) * _info.channels;
                shouldExit = !stream(buffer);
            }

            if (shouldExit) return false;
        }
        /// 1 API-specific function ///
        alSourceQueueBuffers(_source, 1, &buffer);
    }

    return true;
}
#endif