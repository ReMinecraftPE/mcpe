#pragma once

#include "renderer/hal/TextureDescription.hpp"
#include "renderer/hal/enums/ShaderStagesBits.hpp"

namespace mce
{
    class RenderContext;
    class Texture;
    class TextureBase
    {
    public:
        TextureDescription m_description;

    protected:
        bool m_bCreated;
        bool m_bHasWriteAccess;

    public:
		TextureBase();

    protected:
        void _bindWriteBuffer(RenderContext& context);
        void _releaseWriteBuffer(RenderContext& context);

	public:
        const TextureDescription& getDescription() const;
        void deleteTexture();
        void bindTexture(RenderContext& context, unsigned int textureUnit = 0, unsigned int shaderStagesBits = SHADER_STAGE_BIT_PIXEL);
        bool isLoaded() const;

        void convertToMipmapedTexture(unsigned int mipmaps);
        void convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps);

        void enableWriteMode(RenderContext& context);
        void disableWriteMode(RenderContext& context);

        void subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level);
        void subBuffer(RenderContext& context, const void* pixels);

        void copyTexture(RenderContext& context, const Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height);

        void createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level);
        
        void createTexture(const TextureDescription& description);
        void createTexture(RenderContext& context, const TextureDescription& description);

        void lock(RenderContext& context);
        void unlock(RenderContext& context);

        void move(TextureBase& other);

        static bool supportsMipMaps();
    };
}
