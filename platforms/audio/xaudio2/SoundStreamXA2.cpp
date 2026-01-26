#include "SoundStreamXA2.hpp"
#include "common/Logger.hpp"

SoundStreamXA2::SoundStreamXA2(const mce::ComInterface<IXAudio2>& xaudio)
    : m_xaudio(xaudio)
    , m_sourceVoice(nullptr)
    , m_nextBufferIndex(0)
{
    // Zero out WAVEFORMATEX
    ZeroMemory(&m_wfx, sizeof(WAVEFORMATEX));

    // Set initial volume
    _setVolume(getVolume());
}

SoundStreamXA2::~SoundStreamXA2()
{
    _destroyVoice();
}

void SoundStreamXA2::_createVoice()
{
    if (m_sourceVoice) return; // Already exists

    // Setup format based on Vorbis info
    m_wfx.wFormatTag = WAVE_FORMAT_PCM;
    m_wfx.nChannels = (WORD)m_info.channels;
    m_wfx.nSamplesPerSec = m_info.sample_rate;
    m_wfx.wBitsPerSample = 16;
    m_wfx.nBlockAlign = (m_wfx.nChannels * m_wfx.wBitsPerSample) / 8;
    m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
    m_wfx.cbSize = 0;

    if (FAILED(m_xaudio->CreateSourceVoice(&m_sourceVoice, &m_wfx)))
    {
        // Handle error (log it)
        m_sourceVoice = nullptr;
    }
    else
    {
        // Apply volume immediately
        m_sourceVoice->SetVolume(m_volume);
    }
}

void SoundStreamXA2::_destroyVoice()
{
    if (m_sourceVoice)
    {
        m_sourceVoice->Stop(0);
        m_sourceVoice->FlushSourceBuffers();
        m_sourceVoice->DestroyVoice();
        m_sourceVoice = nullptr;
    }
}

void SoundStreamXA2::_setVolume(float vol)
{
    if (m_sourceVoice)
    {
        m_sourceVoice->SetVolume(vol);
    }
}

void SoundStreamXA2::_play()
{
    if (m_sourceVoice)
    {
        m_sourceVoice->Start(0);
    }
}

void SoundStreamXA2::_pause()
{
    if (m_sourceVoice)
    {
        m_sourceVoice->Stop(0);
    }
}

bool SoundStreamXA2::_open(const std::string& fileName)
{
    _createVoice();

    if (!m_sourceVoice) return false;

    m_nextBufferIndex = 0;

    // Prefill both buffers to start the stream
    // We try to fill buffer 0, then buffer 1.
    for (int i = 0; i < 2; ++i)
    {
        if (!_stream(m_nextBufferIndex))
        {
            // If we fail to stream immediately (empty file?), stop.
            // If i==1, we managed one buffer, so strictly speaking we could play,
            // but usually we want to fill the pipe.
            if (i == 0) return false;
        }
        m_nextBufferIndex = (m_nextBufferIndex + 1) % 2;
    }

    _play();

    return true;
}

void SoundStreamXA2::_close()
{
    _destroyVoice();
    // Clearing vectors is not strictly necessary as they resize on next use,
    // but good for housekeeping.
    m_localBuffers[0].clear();
    m_localBuffers[1].clear();
}

void SoundStreamXA2::_update()
{
    if (!m_sourceVoice) return;

    XAUDIO2_VOICE_STATE state;
    m_sourceVoice->GetState(&state);

    // XAudio2 allows queuing multiple buffers. We use 2.
    // If fewer than 2 are queued, it means one (or both) has finished playing.
    while (state.BuffersQueued < 2)
    {
        // _stream fills m_tempPcmBuffer via Vorbis, then calls _publishBuffer
        // which copies data to m_localBuffers[m_nextBufferIndex] and submits to XAudio2.
        if (!_stream(m_nextBufferIndex))
        {
            // Decoding finished or failed
            bool shouldExit = true;

            if (shouldLoop())
            {
                stb_vorbis_seek_start(m_decoder);
                m_totalSamplesLeft = stb_vorbis_stream_length_in_samples(m_decoder) * m_info.channels;

                // Try to fill the buffer again now that we looped
                shouldExit = !_stream(m_nextBufferIndex);
            }

            if (shouldExit)
            {
                // If we ran out of data and aren't looping, 
                // and the queue is empty, we are done.
                if (state.BuffersQueued == 0)
                {
                    close();
                }
                return;
            }
        }

        // Advance to the next buffer index for the next pass
        m_nextBufferIndex = (m_nextBufferIndex + 1) % 2;

        // Update state to check if we need to fill another one immediately
        m_sourceVoice->GetState(&state);
    }
}

void SoundStreamXA2::_publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer)
{
    if (!m_sourceVoice) return;

    // Resize local storage to match the decoded data size
    // We use std::vector to ensure we own the memory passed to XAudio2
    std::vector<int16_t>& destVec = m_localBuffers[destBufferId];

    // Safety check on size
    if (sourceBuffer.m_dataSize <= 0) return;

    // Copy data from the temporary decoding buffer to our persistent buffer
    destVec.resize(sourceBuffer.m_dataSize);

    // Standard memcpy for speed
    memcpy(&destVec[0], sourceBuffer.m_pData, sourceBuffer.m_dataSize * sizeof(int16_t));

    // Submit to XAudio2
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = (UINT32)(destVec.size() * sizeof(int16_t));
    buffer.pAudioData = (const BYTE*)&destVec[0];
    buffer.Flags = 0; // No XAUDIO2_END_OF_STREAM needed for infinite streaming usually, or handle in close

    // SubmitSourceBuffer points to the memory in m_localBuffers.
    // XAudio2 will read this asynchronously. We must not touch this specific 
    // m_localBuffers index until XAudio2 is done with it (checked in _update).
    HRESULT hr = m_sourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
        LOG_E("SubmitSourceBuffer failed: 0x%08x", hr);
    }
}