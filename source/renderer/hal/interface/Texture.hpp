#pragma once

#include "renderer/hal/ogl/TextureOGL.hpp"

namespace mce
{
    class Texture : public TextureOGL
    {
    public:
        Texture();

    public:
        void loadMipMap(RenderContext& context, const void* rawTextureData, unsigned int mipMapLevel);
    };
}