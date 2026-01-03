#include <typeinfo>

#include "TextureD3D11.hpp"
#include "API_D3D11.hpp"
#include "renderer/hal/helpers/TextureHelper.hpp"
#include "renderer/hal/dxgi/helpers/TextureHelperDXGI.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"
#include "renderer/hal/d3d11/helpers/TextureHelperD3D11.hpp"
#include "renderer/hal/interface/Texture.hpp"

using namespace mce;

TextureD3D11::TextureD3D11()
    : TextureBase()
{
    m_writeBuffer = nullptr;
    m_stagingTexture = nullptr;
}

void TextureD3D11::_bindWriteBuffer(RenderContext& context)
{
    TextureBase::_bindWriteBuffer(context);

    D3D11_MAPPED_SUBRESOURCE mappedSubresource = { 0 };
    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        HRESULT hResult = d3dDeviceContext->Map(**m_texture2D, 0, D3D11_MAP_WRITE, 0x0, &mappedSubresource);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    m_writeBuffer = mappedSubresource.pData;
}

void TextureD3D11::_releaseWriteBuffer(RenderContext& context)
{
    TextureBase::_releaseWriteBuffer(context);

    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    d3dDeviceContext->Unmap(**m_texture2D, 0);

    m_writeBuffer = nullptr;
}

void TextureD3D11::deleteTexture()
{
    m_texture2D.release();
    m_samplerState.release();
    m_shaderResourceView.release();

    TextureBase::deleteTexture();
}

void TextureD3D11::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();

    d3dDeviceContext->PSSetShaderResources(textureUnit, 1, *m_shaderResourceView);
    d3dDeviceContext->PSSetSamplers(textureUnit, 1, *m_samplerState);
}

void TextureD3D11::convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps)
{
    // @NOTE: mipmaps was hardcoded to 5 on DX11 for some reason

    if (mipmaps == 0)
    {
        LOG_E("You must have a positive number of mip maps that is greater than 1");
        throw std::bad_cast();
    }

    if (m_description.mipCount == mipmaps)
        return;

    TextureBase::convertToMipmapedTexture(mipmaps);

    TextureD3D11 mipmapedTexture;
    mipmapedTexture.createTexture(context, m_description);

    CD3D11_BOX srcBox;
    {
        srcBox.right = m_description.width;
        srcBox.top = 0;
        srcBox.bottom = m_description.height;
        srcBox.back = 1;
        srcBox.front = 0;
    }

    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        d3dDeviceContext->CopySubresourceRegion(**mipmapedTexture.m_texture2D, 0, 0, 0, 0, **m_texture2D, 0, &srcBox);
    }

    m_description = mipmapedTexture.m_description;
}

void TextureD3D11::enableWriteMode(RenderContext& context)
{
    TextureBase::enableWriteMode(context);

    Texture* texture = new Texture();
    {
        TextureDescription desc(m_description);
        {
            desc.bIsStaging = true;
        }

        texture->createTexture(context, desc);
        // Smuggles the current version of the texture off of the GPU so we can update it
        texture->copyTexture(context, (const Texture*)this, 0, 0, desc.width, desc.height);
        texture->_bindWriteBuffer(context);
    }

    m_stagingTexture = texture;
}

void TextureD3D11::disableWriteMode(RenderContext& context)
{
    TextureBase::disableWriteMode(context);

    m_stagingTexture->_releaseWriteBuffer(context);
    copyTexture(context, m_stagingTexture, 0, 0, m_description.width, m_description.height);

    delete m_stagingTexture;
}

void TextureD3D11::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    // Just write to our staging texture, if we have one
    if (m_stagingTexture != nullptr)
    {
        m_stagingTexture->subBuffer(context, pixels, xoffset, yoffset, width, height, level);
        return;
    }

    TextureBase::subBuffer(context, pixels, xoffset, yoffset, width, height, level);

    uint32_t* writeBuffer = (uint32_t*)m_writeBuffer;

    const uint32_t* pixelPtr = (const uint32_t*)pixels;

    for (int y = yoffset; y < yoffset + height; y++)
    {
        for (int x = xoffset; x < xoffset + width; x++)
        {
            int destIndex = x + y * m_description.width;
            uint32_t color = *pixelPtr++;

            writeBuffer[destIndex] = color;
        }
    }
}

void TextureD3D11::subBuffer(RenderContext& context, const void* pixels)
{
    TextureBase::subBuffer(context, pixels);

    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    unsigned int byteStride = TextureHelper::textureFormatToByteStride(m_description.textureFormat);
    d3dDeviceContext->UpdateSubresource(**m_texture2D, 0, NULL, pixels, m_description.width * byteStride, 0);
}

void TextureD3D11::copyTexture(RenderContext& context, const Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height)
{
    const TextureD3D11* src2 = (const TextureD3D11*)src;
    D3D11_BOX srcBox;
    {
        srcBox.left = startX;
        srcBox.top = startY;
        srcBox.front = 0;
        srcBox.right = width + startX;
        srcBox.bottom = height + startY;
        srcBox.back = 1;
    }

    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    d3dDeviceContext->CopySubresourceRegion(**m_texture2D, 0, startX, startY, 0, (ID3D11Texture2D*)src2->m_texture2D.getPtr(), 0, &srcBox);
}

void TextureD3D11::createTexture(RenderContext& context, const TextureDescription& description)
{
    DXGI_FORMAT dxgiFormat = TextureHelperDXGI::DXGIFormatFromTextureFormat(description.textureFormat);

    D3DDevice d3dDevice = context.getD3DDevice();

    // Create new Texture2D
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        {
            textureDesc.Width = description.width;
            textureDesc.Height = description.height;
            textureDesc.MipLevels = description.mipCount;
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            textureDesc.ArraySize = 1;
            textureDesc.Format = dxgiFormat;
            textureDesc.SampleDesc.Count = description.sampleCount;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            textureDesc.CPUAccessFlags = 0x0;
            textureDesc.MiscFlags = 0x0;
            if (description.bIsStaging)
            {
                textureDesc.Usage = D3D11_USAGE_STAGING;
                textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
                textureDesc.BindFlags = 0x0;
            }
        }

        m_texture2D.release();
        HRESULT hResult = d3dDevice->CreateTexture2D(&textureDesc, NULL, *m_texture2D);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    if (!description.bIsStaging)
    {
        D3D11_SAMPLER_DESC samplerDesc;
        {
            memset(&samplerDesc, 0, sizeof(samplerDesc));
            samplerDesc.Filter = TextureHelperD3D11::D3D11FilterFromTextureFilter(description.filteringLevel);
            if (description.bWrap)
            {
                samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            }
            else
            {
                samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            }

            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MinLOD = 0.0f;
            samplerDesc.MaxLOD = 3.4028e38f;
        }

        // Create new SamplerState
        {
            m_samplerState.release();
            HRESULT hResult = d3dDevice->CreateSamplerState(&samplerDesc, *m_samplerState);
            ErrorHandlerDXGI::checkForErrors(hResult);
        }

        // Create new ShaderResourceView
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            {
                shaderResourceViewDesc.Texture1DArray.ArraySize = 0;
                shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                shaderResourceViewDesc.Texture2D.MipLevels = description.mipCount;
                shaderResourceViewDesc.Buffer.FirstElement = 0;
                shaderResourceViewDesc.Format = dxgiFormat;
            }

            m_shaderResourceView.release();
            HRESULT hResult = d3dDevice->CreateShaderResourceView(**m_texture2D, &shaderResourceViewDesc, *m_shaderResourceView);
            ErrorHandlerDXGI::checkForErrors(hResult);
        }
    }
    
    TextureBase::createTexture(context, description);
}

void TextureD3D11::move(TextureD3D11& other)
{
    TextureBase::move(other);
    std::swap(this->m_texture2D, other.m_texture2D);
    std::swap(this->m_samplerState, other.m_samplerState);
    std::swap(this->m_shaderResourceView, other.m_shaderResourceView);
    std::swap(this->m_writeBuffer, other.m_writeBuffer);
}

bool TextureD3D11::supportsMipMaps()
{
    return true;
}