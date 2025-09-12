#include "SoundStream.hpp"

SoundStream::SoundStream()
{
    m_volume = 1.0f;
    m_bIsStreaming = false;
    m_bIsPaused = false;
    m_bShouldLoop = false;

    m_decoder = nullptr;
    m_info = stb_vorbis_info();
    m_totalSamplesLeft = 0;

    m_tempPcmBuffer.m_dataSize = 4096 * 8;
    m_tempPcmBuffer.m_pData = new int16_t[m_tempPcmBuffer.m_dataSize];
}

SoundStream::~SoundStream()
{
    _deleteDecoder();

    delete[] m_tempPcmBuffer.m_pData;
}

void SoundStream::_deleteDecoder()
{
    if (m_decoder != nullptr)
        stb_vorbis_close(m_decoder);
    m_decoder = nullptr;
}

bool SoundStream::_stream(int bufferId)
{
    int size = 0;
    int result = 0;

    while (size < m_tempPcmBuffer.m_dataSize)
    {
        result = stb_vorbis_get_samples_short_interleaved(m_decoder, m_info.channels, (short*)m_tempPcmBuffer.m_pData + size, m_tempPcmBuffer.m_dataSize - size);
        if (result > 0) size += result * m_info.channels;
        else break;
    }

    if (size == 0) return false;

    _publishBuffer(bufferId, m_tempPcmBuffer);
    m_totalSamplesLeft -= size;

    return true;
}

void SoundStream::setVolume(float vol)
{
    if (m_volume == vol)
        return;

    _setVolume(vol);

    m_volume = vol;
}

void SoundStream::setPausedState(bool isPaused)
{
    if (m_bIsPaused == isPaused)
        return;

    if (isPaused == true)
        _pause();
    else
        _play();

    m_bIsPaused = isPaused;
}

bool SoundStream::open(const std::string& fileName)
{
    if (isStreaming())
    {
        close();
    }

    m_decoder = stb_vorbis_open_filename(fileName.c_str(), NULL, NULL);
    if (!m_decoder) return false;
    // Get file info
    m_info = stb_vorbis_get_info(m_decoder);
    
    if (!_open(fileName)) return false;

    setPausedState(false);

    m_totalSamplesLeft = stb_vorbis_stream_length_in_samples(m_decoder) * m_info.channels;
    m_bIsStreaming = true;

    return true;
}

void SoundStream::close()
{
    _close();

    _deleteDecoder();
    m_totalSamplesLeft = 0;
    m_bIsStreaming = false;
}

void SoundStream::update()
{
    if (!isPlaying()) return;

    _update();
}
