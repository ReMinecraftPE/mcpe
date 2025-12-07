#include <typeinfo>
#include "TextureHelper.hpp"
#include "common/Logger.hpp"

using namespace mce;

unsigned int TextureHelper::textureFormatToByteStride(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case TEXTURE_FORMAT_R8G8B8A8_UNORM:
            return 4;
        default:
            LOG_E("Unknown textureFormat: %d", textureFormat);
            throw std::bad_cast();
    }
}