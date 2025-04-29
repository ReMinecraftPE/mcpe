#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb_image/include/stb_vorbis.c"

#include "client/sound/SoundSystem.hpp"

#if 0

class SoundStream
{
private:
    stb_vorbis* _stream;
    stb_vorbis_info _info;

    uint32_t _buffers[2];
    uint32_t _source;

    size_t _bufferSize;

    size_t _totalSamplesLeft;

    bool _shouldLoop;
    
public:
    SoundStream();
    ~SoundStream();

    bool stream(uint32_t buffer);
    bool open(const char* filename);
    bool update();
};
#endif