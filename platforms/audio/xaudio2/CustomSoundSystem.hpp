#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>
#include <map>

#ifdef _XBOX
#include <xtl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// XAudio2 headers
#include <xaudio2.h>
#include <x3daudio.h>

#include "thirdparty/com/ComInterface.hpp"
#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"

#include "SoundStreamXA2.hpp"

#define SOUND_SYSTEM SoundSystemXA2

// Before you ponder what crack the author of this code was smoking, please note that
// this was created primarily by AI, because I did not want to waste my time learning (then forgetting) the ins and outs of the XAudio2 API,
// only to never touch this code ever again.
class SoundSystemXA2 : public SoundSystem
{
private:
    struct VoiceWrapper
    {
        IXAudio2SourceVoice* sourceVoice;
        X3DAUDIO_EMITTER emitter;
        bool is3D;
        bool inUse;
        WAVEFORMATEX format;

        VoiceWrapper() : sourceVoice(NULL), is3D(false), inUse(false) {
            ZeroMemory(&emitter, sizeof(X3DAUDIO_EMITTER));
            ZeroMemory(&format, sizeof(WAVEFORMATEX));
        }

        void reset() {
            inUse = false;
            is3D = false;
        }
    };

    struct FormatKey {
        DWORD sampleRate;
        WORD channels;
        WORD bits;

        bool operator<(const FormatKey& other) const {
            if (sampleRate != other.sampleRate) return sampleRate < other.sampleRate;
            if (channels != other.channels) return channels < other.channels;
            return bits < other.bits;
        }
    };

public:
    SoundSystemXA2();
    ~SoundSystemXA2();

private:
    WAVEFORMATEX _getWaveFormat(const PCMSoundHeader& header, float pitch) const;
    void _cleanSources();
    void _update3D(); // Recalculates spatialization for all active sounds

    VoiceWrapper* _findFreeVoice(const WAVEFORMATEX& format);

public:
    bool isAvailable() override;
    void setListenerPos(const Vec3& pos) override;
    void setListenerAngle(const Vec2& rot) override;

    void setMusicVolume(float vol) override;

    void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;

    void playMusic(const std::string& soundPath) override;
    bool isPlayingMusic() const override;
    void stopMusic() override;
    void pauseMusic(bool state) override;

    void update() override; 

    void startEngine() override;
    void stopEngine() override;

private:
    bool m_available;

    mce::ComInterface<IXAudio2> m_xaudio;
    IXAudio2MasteringVoice* m_pMasteringVoice;

    X3DAUDIO_HANDLE m_x3dInstance;
    X3DAUDIO_LISTENER m_listener;
    DWORD m_channelMask;
    UINT32 m_deviceChannels;

    // Owns the memory for voices
    std::vector<VoiceWrapper*> m_voicePool;

    // Pointers to available voices in the pool
    std::map<FormatKey, std::vector<VoiceWrapper*> > m_freeVoices;

    // Pointers to currently active voices
    std::vector<VoiceWrapper*> m_activeVoices;

    SoundStreamXA2* m_musicStream;
};