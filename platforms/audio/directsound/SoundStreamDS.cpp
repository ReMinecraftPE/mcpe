#include "SoundStreamDS.hpp"
#include "client/app/AppPlatform.hpp"
#include <cassert>
#include <cstring>

SoundStreamDS::SoundStreamDS(LPDIRECTSOUND ds) : 
    m_directsound(ds),
    m_source(nullptr),
    m_bufferSize(0),
    m_totalBufferSize(0),
    m_isPlaying(false)
{
    _createSource();
    _createBuffers();
    _setVolume(getVolume());
}

SoundStreamDS::~SoundStreamDS()
{
    _deleteSource();
    _deleteBuffers();
}

void SoundStreamDS::_deleteSource()
{
    if (m_source)
    {
        m_source->Stop();
        m_source->Release();
        m_source = nullptr;
    }
}

void SoundStreamDS::_createSource()
{
    if (!m_directsound) return;

    // Create a secondary buffer for streaming
    DSBUFFERDESC desc = {};
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    desc.dwBufferBytes = m_totalBufferSize; // Set in _open
    desc.lpwfxFormat = &m_waveFormat;

    HRESULT hr = m_directsound->CreateSoundBuffer(&desc, &m_source, nullptr);
    DS_ERROR_CHECK(hr);
}

void SoundStreamDS::_resetSource()
{
    if (m_source)
    {
        HRESULT hr = m_source->Stop();
        DS_ERROR_CHECK(hr);
        hr = m_source->SetCurrentPosition(0);
        DS_ERROR_CHECK(hr);
    }
}

void SoundStreamDS::_deleteBuffers()
{
    for (std::vector<LPDIRECTSOUNDBUFFER>::iterator it = m_buffers.begin(); it != m_buffers.end(); ++it)
    {
        LPDIRECTSOUNDBUFFER buffer = *it;
        if (buffer)
            buffer->Release();
    }
    m_buffers.clear();
    m_bufferIdMap.clear();
}

void SoundStreamDS::_createBuffers()
{
    if (!m_directsound || m_bufferSize == 0) return;

    assert(m_buffers.empty());

    // Create two streaming buffers
    DSBUFFERDESC desc = {};
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_LOCSOFTWARE;
    desc.dwBufferBytes = m_bufferSize;
    desc.lpwfxFormat = &m_waveFormat;

    for (int i = 0; i < 2; ++i)
    {
        LPDIRECTSOUNDBUFFER buffer = nullptr;
        HRESULT hr = m_directsound->CreateSoundBuffer(&desc, &buffer, nullptr);
        if (SUCCEEDED(hr))
        {
            m_buffers.push_back(buffer);
            m_bufferIdMap[buffer] = i;
        }
        else
            DS_ERROR_CHECK(hr);
    }
}

void SoundStreamDS::_resetBuffers()
{
    _deleteBuffers();
    _createBuffers();
}

void SoundStreamDS::_setVolume(float vol)
{
    if (m_source)
    {
        // DirectSound volume: 0 to -10000 (hundredths of a decibel)
        LONG dsVolume = vol > 0 ? (LONG)(2000.0f * log10(vol)) : DSBVOLUME_MIN;
        HRESULT hr = m_source->SetVolume(dsVolume);
        DS_ERROR_CHECK(hr);
    }
}

void SoundStreamDS::_play()
{
    if (m_source)
    {
        HRESULT hr = m_source->Play(0, 0, DSBPLAY_LOOPING);
        DS_ERROR_CHECK(hr);
        m_isPlaying = SUCCEEDED(hr);
    }
}

void SoundStreamDS::_pause()
{
    if (m_source)
    {
        HRESULT hr = m_source->Stop();
        DS_ERROR_CHECK(hr);
        m_isPlaying = false;
    }
}

bool SoundStreamDS::_open(const std::string& fileName)
{
    // Set up wave format
    m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_waveFormat.nChannels = m_info.channels;
    m_waveFormat.nSamplesPerSec = m_info.sample_rate;
    m_waveFormat.wBitsPerSample = 16;
    m_waveFormat.nBlockAlign = m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
    m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;

    // Align buffer size with m_tempPcmBuffer
    m_bufferSize = m_tempPcmBuffer.m_dataSize * sizeof(int16_t);
    m_totalBufferSize = m_bufferSize * 2; // Two segments

    // Recreate source and buffers
    _deleteSource();
    _deleteBuffers();
    _createSource();
    _createBuffers();

    // Verify buffers were created
    if (m_buffers.size() != 2 || !m_source)
    {
        LOG_E("Failed to create DirectSound buffers or source");
        return false;
    }

    // Fill initial buffers
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        if (!_stream(i)) return false;
    }

    _play();
    return true;
}

void SoundStreamDS::_close()
{
    _resetSource();
    m_isPlaying = false;
}

void SoundStreamDS::_update()
{
    if (!m_source || !m_isPlaying) return;

    // Check if buffer is lost (e.g., when window is minimized)
    HRESULT hr = m_source->Restore();
    if (hr == DSERR_BUFFERLOST)
    {
        LOG_I("Restoring lost DirectSound buffer");
        hr = m_source->Restore();
        DS_ERROR_CHECK(hr);
        if (FAILED(hr)) return;
        // Re-fill buffers after restoration
        for (size_t i = 0; i < m_buffers.size(); ++i)
        {
            if (!_stream(i))
            {
                if (shouldLoop())
                {
                    stb_vorbis_seek_start(m_decoder);
                    m_totalSamplesLeft = stb_vorbis_stream_length_in_samples(m_decoder) * m_info.channels;
                    if (!_stream(i))
                    {
                        close();
                        return;
                    }
                }
                else
                {
                    close();
                    return;
                }
            }
        }
        _play(); // Restart playback
    }

    DWORD playCursor, writeCursor;
    hr = m_source->GetCurrentPosition(&playCursor, &writeCursor);
    DS_ERROR_CHECK(hr);
    if (FAILED(hr)) return;

    // Determine which segment to update
    static DWORD lastWritePos = 0;
    DWORD currentSegment = (writeCursor >= m_bufferSize) ? 1 : 0;

    // Refill the opposite segment if the write cursor has crossed the boundary
    if ((lastWritePos < m_bufferSize && writeCursor >= m_bufferSize) ||
        (lastWritePos >= m_bufferSize && writeCursor < m_bufferSize))
    {
        int bufferId = (currentSegment + 1) % 2;
        if (!_stream(bufferId))
        {
            if (shouldLoop())
            {
                stb_vorbis_seek_start(m_decoder);
                m_totalSamplesLeft = stb_vorbis_stream_length_in_samples(m_decoder) * m_info.channels;
                if (!_stream(bufferId)) {
                    close();
                    return;
                }
            }
            else
            {
                close();
                return;
            }
        }
        lastWritePos = writeCursor;
    }
}

void SoundStreamDS::_publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer)
{
    if (destBufferId >= m_buffers.size() || !m_source) return;

    LPDIRECTSOUNDBUFFER buffer = m_buffers[destBufferId];
    if (!buffer)
    {
        LOG_E("Invalid buffer at index %d", destBufferId);
        return;
    }

    // Lock the temporary buffer to copy data
    void* ptr1 = nullptr;
    void* ptr2 = nullptr;
    DWORD size1 = 0, size2 = 0;
    DWORD dataSize = sourceBuffer.m_dataSize * sizeof(int16_t);

    HRESULT hr = buffer->Lock(0, dataSize, &ptr1, &size1, &ptr2, &size2, 0);
    if (FAILED(hr))
    {
        DS_ERROR_CHECK(hr);
        return;
    }

    // Copy audio data
    memcpy(ptr1, sourceBuffer.m_pData, size1);
    if (ptr2 && size2 > 0)
        memcpy(ptr2, (char*)sourceBuffer.m_pData + size1, size2);

    hr = buffer->Unlock(ptr1, size1, ptr2, size2);
    DS_ERROR_CHECK(hr);

    // Write to the streaming buffer segment
    void* streamPtr1 = nullptr;
    void* streamPtr2 = nullptr;
    DWORD streamSize1 = 0, streamSize2 = 0;
    DWORD offset = destBufferId * m_bufferSize;

    hr = m_source->Lock(offset, dataSize, &streamPtr1, &streamSize1, &streamPtr2, &streamSize2, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(streamPtr1, sourceBuffer.m_pData, streamSize1);
        if (streamPtr2 && streamSize2 > 0)
            memcpy(streamPtr2, (char*)sourceBuffer.m_pData + streamSize1, streamSize2);
        m_source->Unlock(streamPtr1, streamSize1, streamPtr2, streamSize2);
    }
    else
        DS_ERROR_CHECK(hr);
}