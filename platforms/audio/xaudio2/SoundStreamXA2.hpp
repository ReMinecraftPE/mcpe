#pragma once

#include <vector>
#ifdef _XBOX
#include <xtl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <xaudio2.h>

#include "thirdparty/com/ComInterface.hpp"

#include "compat/LegacyCPP.hpp"
#include "client/sound/SoundStream.hpp"

class SoundStreamXA2 : public SoundStream
{
public:
    SoundStreamXA2(const mce::ComInterface<IXAudio2>& xaudio);
    virtual ~SoundStreamXA2();

protected:
    // SoundStream overrides
    // Note: 'override' keyword omitted for C++03 compliance
    void _setVolume(float vol);
    void _play();
    void _pause();
    bool _open(const std::string& fileName);
    void _close();
    void _update();
    void _publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer);

private:
    void _createVoice();
    void _destroyVoice();

private:
    mce::ComInterface<IXAudio2> m_xaudio;
    IXAudio2SourceVoice* m_sourceVoice;
    WAVEFORMATEX m_wfx;

    // XAudio2 reads directly from memory, so we need persistent storage 
    // for the buffers currently submitted to the voice.
    // We use 2 buffers for double-buffering.
    std::vector<int16_t> m_localBuffers[2];

    // Tracks which buffer index (0 or 1) we should write to next
    int m_nextBufferIndex;
};
