#pragma once

#include "renderer/hal/enums/TextureFormat.hpp"

namespace mce
{
    class TextureHelper
    {
    public:
        static unsigned int textureFormatToByteStride(TextureFormat textureFormat);
    };
}