#pragma once

#include "RenderContext.hpp"
#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER(Texture)

namespace mce
{
    class Texture : public MCE_GFX_CLASS(Texture)
    {
    public:
        Texture();

    public:
        void loadMipMap(RenderContext& context, const void* rawTextureData, unsigned int mipMapLevel);
    };
}