#pragma once
#include <windows.h>
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#include <mmsystem.h>
#include <dsound.h>
#include <string>
#include <vector>
#include <memory>
#include "client/sound/SoundStream.hpp"
#include <map>

#define DS_ERROR_CHECK(hr) \
    if (FAILED(hr)) { \
        LOG_I("DirectSound error: HRESULT 0x%08X at %s:%d", hr, __FILE__, __LINE__); \
    }

class SoundStreamDS : public SoundStream
{
public:
    SoundStreamDS();
    ~SoundStreamDS();

protected:
    void _setVolume(float vol) override;
    void _play() override;
    void _pause() override;
    bool _open(const std::string& fileName) override;
    void _close() override;
    void _update() override;
    void _publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer) override;

private:
    void _deleteSource();
    void _createSource();
    void _resetSource();
    void _deleteBuffers();
    void _createBuffers();
    void _resetBuffers();

private:
    LPDIRECTSOUND8 m_ds;                    // DirectSound object
    LPDIRECTSOUNDBUFFER m_primaryBuffer;    // Primary buffer (optional, for format setting)
    LPDIRECTSOUNDBUFFER m_source;           // Secondary buffer acting as the source
    std::vector<LPDIRECTSOUNDBUFFER> m_buffers; // Streaming buffers
    std::map<LPDIRECTSOUNDBUFFER, int> m_bufferIdMap; // Maps buffers to indices
    WAVEFORMATEX m_waveFormat;              // Audio format
    DWORD m_bufferSize;                     // Size of each streaming buffer
    DWORD m_totalBufferSize;                // Total size for streaming buffer
    bool m_isPlaying;                       // Playback state
};
