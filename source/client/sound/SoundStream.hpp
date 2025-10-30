#pragma once

#include <string>

#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb_image/include/stb_vorbis.c"

#include "compat/LegacyCPP.hpp"
#include "client/sound/SoundData.hpp"

class SoundStream
{
protected:
    typedef unsigned int BufferId;

protected:
    float m_volume;
    bool m_bIsStreaming;
    bool m_bIsPaused;
    bool m_bShouldLoop;

    stb_vorbis* m_decoder;
    stb_vorbis_info m_info;
    size_t m_totalSamplesLeft;

    SoundBuffer m_tempPcmBuffer;

public:
    SoundStream();
    virtual ~SoundStream();

private:
    void _deleteDecoder();

protected:
    virtual void _setVolume(float vol) = 0;
    virtual void _play() = 0;
    virtual void _pause() = 0;
    virtual bool _open(const std::string& fileName) = 0;
    virtual void _close() = 0;
    virtual void _update() = 0;
    virtual void _publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer) = 0;

    bool _stream(int bufferId);

public:
    float getVolume() const { return m_volume; }
    void setVolume(float vol);
    bool isStreaming() const { return m_bIsStreaming; }
    bool isPaused() const { return m_bIsPaused; }
    void setPausedState(bool isPaused);
    bool isPlaying() const { return isStreaming() && !isPaused(); }
    bool shouldLoop() const { return m_bShouldLoop; }

    bool open(const std::string& fileName);
    void close();
    void update();
};