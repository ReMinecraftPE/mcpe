#pragma once

#include "renderer/hal/base/TextureBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "thirdparty/com/ComInterface.hpp"

namespace mce
{
    class TextureD3D9 : public TextureBase
    {
    protected:
        ComInterface<IDirect3DTexture9> m_texture;

        // We must not read from this pointer. Doing so will incur a performance penalty, like it's a sport or some shit.
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d9/nf-d3d9-id3d9devicecontext-map#don-t-read-from-a-subresource-mapped-for-writing
        void* m_writeBuffer;
        uint32_t m_writePitch; // D3D9 LockRect provides a pitch we must respect

    public:
        TextureD3D9();

    public:
        void deleteTexture();
        void bindTexture(RenderContext& context, unsigned int textureUnit = 0, unsigned int shaderStagesBits = SHADER_STAGE_BIT_PIXEL);
        
        void convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps);

        void enableWriteMode(RenderContext& context);
        void disableWriteMode(RenderContext& context);

        void subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level);
        void subBuffer(RenderContext& context, const void* pixels);

        void copyTexture(RenderContext& context, const Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height);

        void createTexture(RenderContext& context, const TextureDescription& description);

        void move(TextureD3D9& other);

    public:
        static bool supportsMipMaps();
    };
}
