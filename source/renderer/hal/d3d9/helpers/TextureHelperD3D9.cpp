#include "TextureHelperD3D9.hpp"
#include "common/Logger.hpp"

using namespace mce;

D3DFORMAT TextureHelperD3D9::D3DFormatFromTextureFormat(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
    case TEXTURE_FORMAT_R32G32B32A32_FLOAT:    return D3DFMT_A32B32G32R32F;
    case TEXTURE_FORMAT_R16G16B16A16_FLOAT:    return D3DFMT_A16B16G16R16F;
    case TEXTURE_FORMAT_R16G16B16A16_UNORM:    return D3DFMT_A16B16G16R16;
    case TEXTURE_FORMAT_R32G32_FLOAT:          return D3DFMT_G32R32F;

    // D3D9 sRGB is handled via SamplerStates, not the Format enum
    case TEXTURE_FORMAT_R8G8B8A8_UNORM:        // D3D9 does not have RGBA
    case TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB:   return D3DFMT_A8R8G8B8;

    case TEXTURE_FORMAT_B8G8R8A8_UNORM:
    case TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB:   return D3DFMT_A8R8G8B8;

    case TEXTURE_FORMAT_R16G16_UNORM:          return D3DFMT_G16R16;
    case TEXTURE_FORMAT_R16G16_UINT:           return D3DFMT_G16R16; // No integer-only equivalent

    case TEXTURE_FORMAT_D32_FLOAT:             return D3DFMT_D32;
    case TEXTURE_FORMAT_R32_FLOAT:             return D3DFMT_R32F;
    case TEXTURE_FORMAT_R32_UINT:              return D3DFMT_R32F; // No integer-only equivalent

    case TEXTURE_FORMAT_R24G8_TYPELESS:        return D3DFMT_D24S8;
    case TEXTURE_FORMAT_D24_UNORM_S8_UINT:     return D3DFMT_D24S8;
    case TEXTURE_FORMAT_R24_UNORM_X8_TYPELESS: return D3DFMT_D24X8;

    case TEXTURE_FORMAT_R8_UNORM:              return D3DFMT_L8; // Luminance 8-bit
    case TEXTURE_FORMAT_R8_UINT:               return D3DFMT_L8;
    case TEXTURE_FORMAT_A8_UNORM:              return D3DFMT_A8;

    case TEXTURE_FORMAT_BC3_UNORM:             return D3DFMT_DXT5;

    case TEXTURE_FORMAT_BC7_UNORM:
        LOG_W("BC7 is not supported in D3D9");
        return D3DFMT_UNKNOWN;

    default:
        LOG_W("Unknown textureFormat: %d", textureFormat);
        return D3DFMT_UNKNOWN;
    }
}

TextureFormat TextureHelperD3D9::TextureFormatFromD3DFormat(D3DFORMAT d3dFormat)
{
    switch (d3dFormat)
    {
    case D3DFMT_A32B32G32R32F: return TEXTURE_FORMAT_R32G32B32A32_FLOAT;
    case D3DFMT_A16B16G16R16F: return TEXTURE_FORMAT_R16G16B16A16_FLOAT;
    case D3DFMT_A16B16G16R16:  return TEXTURE_FORMAT_R16G16B16A16_UNORM;
    case D3DFMT_G32R32F:       return TEXTURE_FORMAT_R32G32_FLOAT;
    case D3DFMT_A8R8G8B8:      return TEXTURE_FORMAT_R8G8B8A8_UNORM;
    case D3DFMT_G16R16:        return TEXTURE_FORMAT_R16G16_UNORM;
    case D3DFMT_R32F:          return TEXTURE_FORMAT_R32_FLOAT;
    case D3DFMT_D32:           return TEXTURE_FORMAT_D32_FLOAT;
    case D3DFMT_D24S8:         return TEXTURE_FORMAT_D24_UNORM_S8_UINT;
    case D3DFMT_D24X8:         return TEXTURE_FORMAT_R24_UNORM_X8_TYPELESS;
    case D3DFMT_L8:            return TEXTURE_FORMAT_R8_UNORM;
    case D3DFMT_A8:            return TEXTURE_FORMAT_A8_UNORM;
    case D3DFMT_DXT5:          return TEXTURE_FORMAT_BC3_UNORM;
    case D3DFMT_DXT1:         /* We have no BC1 */
    default:
        LOG_W("Unknown d3dFormat: %d", d3dFormat);
        return TEXTURE_FORMAT_UNKNOWN;
    }
}