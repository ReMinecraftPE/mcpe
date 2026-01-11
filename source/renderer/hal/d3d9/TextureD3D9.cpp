#include <typeinfo>

#include "TextureD3D9.hpp"
#include "API_D3D9.hpp"
#include "renderer/hal/helpers/TextureHelper.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"
#include "renderer/hal/d3d9/helpers/TextureHelperD3D9.hpp"
#include "renderer/hal/interface/Texture.hpp"

using namespace mce;

TextureD3D9::TextureD3D9()
    : TextureBase()
{
    m_writeBuffer = nullptr;
    m_writePitch = 0;
}

void TextureD3D9::deleteTexture()
{
    m_texture.release();

    TextureBase::deleteTexture();
}

void TextureD3D9::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
    D3DDevice d3dDevice = context.getD3DDevice();

    // Set the Texture
    d3dDevice->SetTexture(textureUnit, **m_texture);

    // Address Modes (Wrap/Clamp)
    D3DTEXTUREADDRESS addr = m_description.bWrap ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
    d3dDevice->SetSamplerState(textureUnit, D3DSAMP_ADDRESSU, addr);
    d3dDevice->SetSamplerState(textureUnit, D3DSAMP_ADDRESSV, addr);

    // Detailed Filtering Mapping
    D3DTEXTUREFILTERTYPE minFilter = D3DTEXF_LINEAR;
    D3DTEXTUREFILTERTYPE magFilter = D3DTEXF_LINEAR;
    D3DTEXTUREFILTERTYPE mipFilter = D3DTEXF_NONE;

    switch (m_description.filteringLevel)
    {
    case TEXTURE_FILTERING_POINT:
        minFilter = D3DTEXF_POINT;
        magFilter = D3DTEXF_POINT;
        mipFilter = D3DTEXF_POINT;
        break;

    case TEXTURE_FILTERING_BILINEAR:
        minFilter = D3DTEXF_LINEAR;
        magFilter = D3DTEXF_LINEAR;
        mipFilter = D3DTEXF_NONE; // Bilinear = No Mips
        break;

    case TEXTURE_FILTERING_MIPMAP_BILINEAR:
        minFilter = D3DTEXF_LINEAR;
        magFilter = D3DTEXF_LINEAR;
        mipFilter = D3DTEXF_POINT; // Mipmap Bilinear = Linear within level, Point between levels
        break;

    case TEXTURE_FILTERING_TRILINEAR:
    case TEXTURE_FILTERING_TEXEL_AA:
    case TEXTURE_FILTERING_PCF:
        minFilter = D3DTEXF_LINEAR;
        magFilter = D3DTEXF_LINEAR;
        mipFilter = D3DTEXF_LINEAR; // Trilinear = Linear within level AND between levels
        break;
    }

    // Hardware check: If the texture has no mips, we must disable mip filtering
    if (m_description.mipCount <= 1)
    {
        mipFilter = D3DTEXF_NONE;
    }

    d3dDevice->SetSamplerState(textureUnit, D3DSAMP_MINFILTER, minFilter);
    d3dDevice->SetSamplerState(textureUnit, D3DSAMP_MAGFILTER, magFilter);
    d3dDevice->SetSamplerState(textureUnit, D3DSAMP_MIPFILTER, mipFilter);

    // Support for Anisotropy (if the engine eventually adds it to the enum)
    // d3dDevice->SetSamplerState(textureUnit, D3DSAMP_MAXANISOTROPY, 1);
}

void TextureD3D9::convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps)
{
    if (mipmaps == 0)
    {
        LOG_E("You must have a positive number of mip maps that is greater than 1");
        throw std::bad_cast();
    }

    if (m_description.mipCount == mipmaps)
        return;

    TextureBase::convertToMipmapedTexture(mipmaps);

    TextureD3D9 mipmapedTexture;
    mipmapedTexture.createTexture(context, m_description);

    // Have no clue if this will actually work as intended, but right now we don't support mipmaps, so idk
    mipmapedTexture.copyTexture(context, (const Texture*)this, 0, 0, m_description.width, m_description.height);

    m_texture = mipmapedTexture.m_texture;
    m_description = mipmapedTexture.m_description;
}

void TextureD3D9::enableWriteMode(RenderContext& context)
{
    TextureBase::enableWriteMode(context);

    D3DLOCKED_RECT lockedRect;
    {
        HRESULT hResult = m_texture->LockRect(0, &lockedRect, NULL, 0x0);
        ErrorHandlerD3D9::checkForErrors(hResult);
    }

    m_writeBuffer = lockedRect.pBits;
    m_writePitch = lockedRect.Pitch;
}

void TextureD3D9::disableWriteMode(RenderContext& context)
{
    TextureBase::disableWriteMode(context);

    m_texture->UnlockRect(0);
    m_writeBuffer = nullptr;
    m_writePitch = 0;
}

void TextureD3D9::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    TextureBase::subBuffer(context, pixels, xoffset, yoffset, width, height, level);

    if (!TextureHelper::isTextureFormatRGBA(m_description.textureFormat))
    {
        LOG_E("Unsupported TextureFormat: %d", m_description.textureFormat);
        throw std::bad_cast();
    }

    unsigned int stride = TextureHelper::textureFormatToByteStride(m_description.textureFormat);

    uint8_t* writeBuffer = (uint8_t*)m_writeBuffer;
    const uint8_t* pixelPtr = (const uint8_t*)pixels;

    for (unsigned int y = yoffset; y < yoffset + height; y++)
    {
        // Calculate the start of the destination row using PITCH, not width
        uint8_t* destRow = writeBuffer + (y * m_writePitch);

        for (unsigned int x = xoffset; x < xoffset + width; x++)
        {
            // RGBA -> ARGB (big-endian)
            uint8_t color[4];
/*#if MC_ENDIANNESS_BIG
            color[0] = pixelPtr[2]; // R <= A
            color[1] = pixelPtr[0]; // G <= R
            color[2] = pixelPtr[1]; // B <= G
            color[3] = pixelPtr[3]; // A <= B
#else // MC_ENDIANNESS_LITTLE*/
            color[0] = pixelPtr[2]; // R <= B
            color[1] = pixelPtr[1]; // G <= G
            color[2] = pixelPtr[0]; // B <= R
            color[3] = pixelPtr[3]; // A <= A
//#endif

            pixelPtr += stride;

            memcpy(&destRow[x * stride], color, stride);
        }
    }
}

void TextureD3D9::subBuffer(RenderContext& context, const void* pixels)
{
    // @NOTE: We have to bind and release our write buffer here
    // because our base class has no expectation of it already being bound
    // However, we're not likely to need to re-use this write buffer anyways
    enableWriteMode(context);
    TextureD3D9::subBuffer(context, pixels, 0, 0, m_description.width, m_description.height, 0);
    disableWriteMode(context);
}

void TextureD3D9::copyTexture(RenderContext& context, const Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height)
{
    LOG_E("TextureD3D9::copyTexture is not implemented!");
    throw std::bad_cast();

    /*const TextureD3D9* src2 = (const TextureD3D9*)src;
    D3DDevice d3dDevice = context.getD3DDevice();

    ComInterface<IDirect3DSurface9> srcSurface;
    ComInterface<IDirect3DSurface9> destSurface;

    m_texture->GetSurfaceLevel(0, *destSurface);
    ((IDirect3DTexture9*)src2->m_texture.getPtr())->GetSurfaceLevel(0, *srcSurface);

    RECT srcRect;
    {
        srcRect.left = startX;
        srcRect.top = startY;
        srcRect.right = width + startX;
        srcRect.bottom = height + startY;
    }
    POINT destPoint = { startX, startY };

    // @TODO: Only works on Windows
    d3dDevice->UpdateSurface(**srcSurface, &srcRect, **destSurface, &destPoint);*/
}

void TextureD3D9::createTexture(RenderContext& context, const TextureDescription& description)
{
    D3DFORMAT d3dFormat = TextureHelperD3D9::D3DFormatFromTextureFormat(description.textureFormat);

    D3DDevice d3dDevice = context.getD3DDevice();

    m_texture.release();
    HRESULT hr = d3dDevice->CreateTexture(
        description.width,
        description.height,
        description.mipCount,
        D3DUSAGE_DYNAMIC, // 360 doesn't respect this, and we're never gonna be running D3D9 on Windows
        d3dFormat,
        D3DPOOL_DEFAULT,
        *m_texture,
        NULL
    );
    
    TextureBase::createTexture(context, description);
}

void TextureD3D9::move(TextureD3D9& other)
{
    TextureBase::move(other);
    std::swap(this->m_texture, other.m_texture);
    std::swap(this->m_writeBuffer, other.m_writeBuffer);
    std::swap(this->m_writePitch, other.m_writePitch);
}

bool TextureD3D9::supportsMipMaps()
{
    return true;
}
