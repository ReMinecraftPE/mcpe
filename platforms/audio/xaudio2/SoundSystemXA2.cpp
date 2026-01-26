#include "CustomSoundSystem.hpp"
#include "common/Logger.hpp"
#include "common/Utils.hpp"
#include "client/app/AppPlatform.hpp"
#include <cmath>

SoundSystemXA2::SoundSystemXA2()
    : m_available(false)
    , m_pMasteringVoice(nullptr)
    , m_channelMask(0)
    , m_deviceChannels(0)
    , m_musicStream(nullptr)
{
    // Initialize default listener
    ZeroMemory(&m_listener, sizeof(X3DAUDIO_LISTENER));
    m_listener.OrientFront.z = 1.0f;
    m_listener.OrientTop.y = 1.0f;

    startEngine();
}

SoundSystemXA2::~SoundSystemXA2()
{
    stopEngine();
}

WAVEFORMATEX SoundSystemXA2::_getWaveFormat(const PCMSoundHeader& header, float pitch) const
{
    WAVEFORMATEX wf;
    ZeroMemory(&wf, sizeof(WAVEFORMATEX));

    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = header.m_channels;
    wf.nSamplesPerSec = header.m_sample_rate; // Pitch is handled via SetFrequencyRatio in XA2
    wf.wBitsPerSample = 8 * header.m_bytes_per_sample;
    wf.nBlockAlign = (wf.wBitsPerSample / 8) * wf.nChannels;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    wf.cbSize = 0;

    return wf;
}

SoundSystemXA2::VoiceWrapper* SoundSystemXA2::_findFreeVoice(const WAVEFORMATEX& wf)
{
    FormatKey key;
    key.sampleRate = wf.nSamplesPerSec;
    key.channels = wf.nChannels;
    key.bits = wf.wBitsPerSample;

    std::map<FormatKey, std::vector<VoiceWrapper*> >::iterator it = m_freeVoices.find(key);

    if (it != m_freeVoices.end() && !it->second.empty())
    {
        VoiceWrapper* wrapper = it->second.back();
        it->second.pop_back();

        wrapper->sourceVoice->FlushSourceBuffers();
        wrapper->sourceVoice->SetVolume(1.0f);
        wrapper->sourceVoice->SetFrequencyRatio(1.0f);

        return wrapper;
    }

    VoiceWrapper* newWrapper = new VoiceWrapper();
    newWrapper->format = wf;

    HRESULT hr = m_xaudio->CreateSourceVoice(&newWrapper->sourceVoice, &wf);
    if (FAILED(hr))
    {
        LOG_E("Failed to create XAudio2 Source Voice");
        delete newWrapper;
        return NULL;
    }

    m_voicePool.push_back(newWrapper);
    return newWrapper;
}

// Release sounds that finished playing
void SoundSystemXA2::_cleanSources()
{
    // Iterate backwards so we can erase efficiently
    for (int i = (int)m_activeVoices.size() - 1; i >= 0; i--)
    {
        VoiceWrapper* v = m_activeVoices[i];
        XAUDIO2_VOICE_STATE state;
        v->sourceVoice->GetState(&state);

        if (state.BuffersQueued == 0)
        {
            v->sourceVoice->Stop();

            FormatKey key;
            key.sampleRate = v->format.nSamplesPerSec;
            key.channels = v->format.nChannels;
            key.bits = v->format.wBitsPerSample;

            m_activeVoices.erase(m_activeVoices.begin() + i);
            m_freeVoices[key].push_back(v);
        }
    }
}

bool SoundSystemXA2::isAvailable()
{
    return m_available;
}

void SoundSystemXA2::setListenerPos(const Vec3& pos)
{
    if (!isAvailable()) return;

    // Update X3D Listener position
    // Note: We likely uses Y-up. XAudio2 uses standard LHS (Y-up, Z-forward) usually.
    m_listener.Position.x = pos.x;
    m_listener.Position.y = pos.y;
    m_listener.Position.z = -pos.z;

    // Recalculate 3D effects for all currently playing sounds
    _update3D();
}

void SoundSystemXA2::setListenerAngle(const Vec2& rot)
{
    if (!isAvailable()) return;

    float yaw = rot.x * MTH_DEG_TO_RAD;
    float pitch = rot.y * MTH_DEG_TO_RAD;

    // Calculate Look/Front Vector
    float lx = Mth::cos(pitch) * Mth::sin(yaw);
    float ly = -Mth::sin(pitch);
    float lz = Mth::cos(yaw);

    // Calculate Up Vector
    float ux = Mth::sin(pitch) * Mth::sin(yaw);
    float uy = Mth::cos(pitch);
    float uz = Mth::sin(pitch) * Mth::cos(yaw);

    // Invert Z on orientation vectors to match Position's Z inversion logic
    m_listener.OrientFront.x = -lx;
	m_listener.OrientFront.y = -ly;
	m_listener.OrientFront.z = -lz;

    m_listener.OrientTop.x = ux;
	m_listener.OrientTop.y = uy;
	m_listener.OrientTop.z = uz;

    // Recalculate 3D effects
    _update3D();
}

void SoundSystemXA2::setMusicVolume(float vol)
{
    assert(m_musicStream != nullptr);

    m_musicStream->setVolume(vol);
}

// Helper to update spatialization for all active sounds
void SoundSystemXA2::_update3D()
{
    if (m_activeVoices.empty()) return;

    X3DAUDIO_DSP_SETTINGS dsp;
    ZeroMemory(&dsp, sizeof(X3DAUDIO_DSP_SETTINGS));
    float matrix[XAUDIO2_MAX_AUDIO_CHANNELS] = { 0 };

    dsp.DstChannelCount = m_deviceChannels;
    dsp.pMatrixCoefficients = matrix;

    for (size_t i = 0; i < m_activeVoices.size(); i++)
    {
        VoiceWrapper* voice = m_activeVoices[i];
        if (voice->is3D)
        {
            dsp.SrcChannelCount = 1;
            X3DAudioCalculate(m_x3dInstance, &m_listener, &voice->emitter,
                X3DAUDIO_CALCULATE_MATRIX, &dsp);

            voice->sourceVoice->SetOutputMatrix(NULL, 1, m_deviceChannels, dsp.pMatrixCoefficients);
        }
    }
}

void SoundSystemXA2::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
    if (!isAvailable()) return;

    _cleanSources();

    // 1. Get Format and Voice
    WAVEFORMATEX wf = _getWaveFormat(sound.m_header, 1.0f);
    VoiceWrapper* voice = _findFreeVoice(wf);
    if (!voice) return;

    // 3. Apply Pitch and Volume
    if (pitch > 0.0f) voice->sourceVoice->SetFrequencyRatio(pitch);
    voice->sourceVoice->SetVolume(Mth::clamp(volume, 0.0f, 1.0f));

    // 4. Determine 2D vs 3D
    bool is2D = (pos.length() == 0.0f);

    // Only use X3D (Spatialization) if the sound is Mono and not at (0,0,0)
    // Stereo sounds cannot be spatially positioned effectively by X3DAudio's standard emitter.
    if (!is2D && sound.m_header.m_channels == 1)
    {
        voice->is3D = true;
        voice->emitter.ChannelCount = 1;
        voice->emitter.CurveDistanceScaler = 1.0f;

        // Match DirectSound coordinate inversion (Z inverted)
        voice->emitter.Position.x = pos.x;
        voice->emitter.Position.y = pos.y;
        voice->emitter.Position.z = -pos.z;

        X3DAUDIO_DSP_SETTINGS dsp;
        ZeroMemory(&dsp, sizeof(X3DAUDIO_DSP_SETTINGS));
        float matrix[8]; // Sufficient for Mono -> 7.1
        ZeroMemory(matrix, sizeof(matrix));

        dsp.SrcChannelCount = 1;
        dsp.DstChannelCount = m_deviceChannels;
        dsp.pMatrixCoefficients = matrix;

        X3DAudioCalculate(m_x3dInstance, &m_listener, &voice->emitter,
            X3DAUDIO_CALCULATE_MATRIX, &dsp);

        voice->sourceVoice->SetOutputMatrix(NULL, 1, m_deviceChannels, dsp.pMatrixCoefficients);
    }
    else
    {
        // === FIXED 2D LOGIC ===
        voice->is3D = false;

        // We must calculate a proper mapping matrix for 2D sounds.
        // Array size must be: SourceChannels * DeviceChannels
        // Max safe assumption: 2 (Stereo Source) * 8 (7.1 Device) = 16 floats.
        float matrix[16];
        ZeroMemory(matrix, sizeof(matrix));

        if (wf.nChannels == 1)
        {
            // MONO SOURCE: Play on Front Left (0) and Front Right (1)
            // Even if device is 7.1, we typically only want L/R for UI.
            if (m_deviceChannels >= 1) matrix[0] = 1.0f; // Left
            if (m_deviceChannels >= 2) matrix[1] = 1.0f; // Right
        }
        else if (wf.nChannels == 2)
        {
            // STEREO SOURCE: Map Left->Left, Right->Right
            // Matrix layout: [Src0Dest0, Src0Dest1, ... Src1Dest0, Src1Dest1...]

            // Source 0 (Left) -> Dest 0 (Left)
            if (m_deviceChannels >= 1) matrix[0] = 1.0f;

            // Source 1 (Right) -> Dest 1 (Right)
            // Index = (SourceIndex * DestCount) + DestIndex
            // Index = (1 * m_deviceChannels) + 1
            if (m_deviceChannels >= 2) matrix[m_deviceChannels + 1] = 1.0f;
        }

        // Apply the matrix. This resets any previous 3D panning if the voice was reused.
        voice->sourceVoice->SetOutputMatrix(NULL, wf.nChannels, m_deviceChannels, matrix);
    }

    // 5. Submit and Play
    XAUDIO2_BUFFER buffer;
    ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
    buffer.AudioBytes = sound.m_buffer.m_dataSize;
    buffer.pAudioData = (const BYTE*)sound.m_buffer.m_pData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    voice->sourceVoice->SubmitSourceBuffer(&buffer);
    voice->sourceVoice->Start(0);

    m_activeVoices.push_back(voice);
}

void SoundSystemXA2::playMusic(const std::string& soundPath)
{
    m_musicStream->open(soundPath);
}

bool SoundSystemXA2::isPlayingMusic() const
{
    return m_musicStream->isPlaying();
}

void SoundSystemXA2::stopMusic()
{
    m_musicStream->close();
}

void SoundSystemXA2::pauseMusic(bool state)
{
    m_musicStream->setPausedState(state);
}

void SoundSystemXA2::update(float elapsedTime)
{
    m_musicStream->update();
    _cleanSources();
}

void SoundSystemXA2::startEngine()
{
    if (isAvailable()) return;

    HRESULT result;

#ifndef _XBOX
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif

    // Create the XAudio2 engine
    result = XAudio2Create(*m_xaudio, 0x0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(result))
    {
        LOG_E("SoundSystemXA2 failed to create XAudio2 engine (err = 0x%08x)!", result);
        return;
    }

#ifdef _DEBUG
    // Enable debug logging for XAudio2
    XAUDIO2_DEBUG_CONFIGURATION debug;
    ZeroMemory(&debug, sizeof(XAUDIO2_DEBUG_CONFIGURATION));
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    m_xaudio->SetDebugConfiguration(&debug, 0);
#endif

    // Create the mastering voice (represents the audio device)
    result = m_xaudio->CreateMasteringVoice(&m_pMasteringVoice);
    if (FAILED(result))
    {
        LOG_E("SoundSystemXA2 failed to create Mastering Voice (err = 0x%08x)!", result);
        return;
    }

    // Initialize X3DAudio (requires channel mask from the device)
    XAUDIO2_VOICE_DETAILS details;
    m_pMasteringVoice->GetVoiceDetails(&details);
    m_deviceChannels = details.InputChannels;

#ifdef _XBOX
	X3DAudioInitialize(SPEAKER_XBOX, X3DAUDIO_SPEED_OF_SOUND, m_x3dInstance);
#else
    DWORD dwChannelMask;
    m_pMasteringVoice->GetChannelMask(&dwChannelMask);

    // Standard channel mask guess if the device doesn't report one strictly
    // (though XAudio2 usually handles this internally, X3DAudio needs a mask)
    result = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_x3dInstance);
    if (FAILED(result))
    {
        // Fallback: Assume stereo if mask is zero
        X3DAudioInitialize(SPEAKER_STEREO, X3DAUDIO_SPEED_OF_SOUND, m_x3dInstance);
    }
#endif

    m_musicStream = new SoundStreamXA2(m_xaudio);

    // Mark as loaded
    m_available = true;
}

void SoundSystemXA2::stopEngine()
{
    if (!isAvailable()) return;

    LOG_I("Destroying SoundSystemXA2");

    delete m_musicStream;

    // Clean up voice pool
    for (size_t i = 0; i < m_voicePool.size(); i++)
    {
        VoiceWrapper* wrapper = m_voicePool[i];
        if (wrapper->sourceVoice)
        {
            wrapper->sourceVoice->Stop();
            wrapper->sourceVoice->DestroyVoice();
        }
        delete wrapper;
    }
    m_voicePool.clear();
    m_activeVoices.clear();
    m_freeVoices.clear();

    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }

    if (m_xaudio)
    {
        m_xaudio->StopEngine();
        m_xaudio.release();
    }

     // Mark as unloaded
    m_available = false;
}