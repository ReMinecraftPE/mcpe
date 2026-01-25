#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "thirdparty/OpenAL.h"

#include "compat/LegacyCPP.hpp"

#include "client/sound/SoundStream.hpp"

class SoundStreamOAL : public SoundStream
{
private:
    typedef std::map<ALuint, unsigned int> BufferIdMap;

public:
    SoundStreamOAL();
    ~SoundStreamOAL();

private:
    void _deleteSource();
    void _createSource();
    void _resetSource();

    void _deleteBuffers();
    void _createBuffers();
    void _resetBuffers();
	
	ALuint* _getBuffersArray();

protected:
    void _setVolume(float vol) override;
    void _play() override;
    void _pause() override;
    bool _open(const std::string& fileName) override;
    void _close() override;
    void _update() override;
    void _publishBuffer(unsigned int destBufferId, const SoundBuffer& sourceBuffer);

private:
    std::vector<ALuint> m_buffers;
    BufferIdMap m_bufferIdMap;

    ALuint m_source;

    ALenum m_alFormat;
};
