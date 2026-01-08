#include <typeinfo>

#include "TextureHelperD3D11.hpp"
#include "common/Logger.hpp"

using namespace mce;

D3D11_FILTER TextureHelperD3D11::D3D11FilterFromTextureFilter(TextureFiltering textureFiltering)
{
    switch (textureFiltering)
    {
    case TEXTURE_FILTERING_POINT:           return D3D11_FILTER_MIN_MAG_MIP_POINT;
    case TEXTURE_FILTERING_BILINEAR:        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case TEXTURE_FILTERING_MIPMAP_BILINEAR: //return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case TEXTURE_FILTERING_TEXEL_AA:        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    case TEXTURE_FILTERING_TRILINEAR:       return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case TEXTURE_FILTERING_PCF:             return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    default:
        LOG_E("Unknown textureFiltering: %d", textureFiltering);
        throw std::bad_cast();
    }
}