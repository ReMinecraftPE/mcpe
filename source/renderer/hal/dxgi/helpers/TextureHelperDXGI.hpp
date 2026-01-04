#pragma once

#include "../API_DXGI.hpp"
#include "renderer/hal/enums/TextureFormat.hpp"

namespace mce
{
    class TextureHelperDXGI
    {
    public:
        static DXGI_FORMAT DXGIFormatFromTextureFormat(TextureFormat textureFormat);
        static TextureFormat TextureFormatFromDXGIFormat(DXGI_FORMAT dxgiFormat);
    };
}