#pragma once

#include "renderer/hal/base/TextureBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"

namespace mce
{
    class TextureD3D11 : public TextureBase
    {
    private:
        ComInterface<ID3D11Texture2D> m_texture2D;
        ComInterface<ID3D11SamplerState> m_samplerState;
        ComInterface<ID3D11ShaderResourceView> m_shaderResourceView;

        // We must not read from this pointer. Doing so will incur a performance penalty, like it's a sport or some shit.
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-map#don-t-read-from-a-subresource-mapped-for-writing
        void* m_writeBuffer;

    public:
        TextureD3D11();

    public:
        void deleteTexture();
        void bindTexture(RenderContext& context, unsigned int textureUnit = 0, unsigned int shaderStagesBits = SHADER_STAGE_BIT_PIXEL);
        
        void convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps);

        void bindWriteBuffer(RenderContext& context);
        void releaseWriteBuffer(RenderContext& context);

        void subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level);
        void subBuffer(RenderContext& context, const void* pixels);

        void createTexture(RenderContext& context, const TextureDescription& description);

        void move(TextureD3D11& other);

    public:
        static bool supportsMipMaps();
    };
}
