#pragma once

#include <stdint.h>

#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb_image/include/stb_vorbis.c"
#include "thirdparty/OpenAL.h"

#include "client/sound/SoundStream.hpp"

class SoundStreamAL : public SoundStream
{
private:
    stb_vorbis* _decoder;
    stb_vorbis_info _info;

    uint32_t _buffers[2];
    uint32_t _source;

    size_t _tempPcmBufferSize;
    int16_t* _tempPcmBuffer;

    ALenum _alFormat;
    size_t _totalSamplesLeft;
    bool _isStreaming;
    bool _isPaused;
    bool _shouldLoop;

public:
    SoundStreamAL();
    ~SoundStreamAL();

private:
    bool _stream(uint32_t buffer);

    void _deleteSource();
    void _createSource();
    void _resetSource();

    void _deleteBuffers();
    void _createBuffers();
    void _resetBuffers();

    void _deleteDecoder();

public:
    bool isStreaming() const override { return _isStreaming; }
    bool isPaused() const override { return _isPaused; }

    bool open(const std::string& flieName) override;
    void close() override;
    void update() override;

    void setPausedState(bool state) override { _isPaused = state; };
    void setVolume(float vol) override;
};