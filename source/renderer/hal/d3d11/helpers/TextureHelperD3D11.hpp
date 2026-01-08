#pragma once

#include "../API_D3D11.hpp"
#include "renderer/hal/enums/TextureFiltering.hpp"

namespace mce
{
    class TextureHelperD3D11
    {
    public:
        static D3D11_FILTER D3D11FilterFromTextureFilter(TextureFiltering textureFiltering);
    };
}