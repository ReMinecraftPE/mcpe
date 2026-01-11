#pragma once

#include "../API_D3D9.hpp"
#include "renderer/hal/enums/TextureFormat.hpp"

namespace mce
{
    class TextureHelperD3D9
    {
    public:
        static D3DFORMAT D3DFormatFromTextureFormat(TextureFormat textureFormat);
        static TextureFormat TextureFormatFromD3DFormat(D3DFORMAT d3dFormat);
    };
}