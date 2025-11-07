#pragma once

#include "renderer/hal/TextureDescription.hpp"

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

    public:
        const TextureDescription& getDescription() const;
        void deleteTexture();
        void bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits);
        bool isLoaded() const;

        void convertToMipmapedTexture(unsigned int mipmaps);
        void convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps);

        void subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level);
        void subBuffer(RenderContext& context, const void* pixels);

        void copyTexture(RenderContext& context, Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height);

        void createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level);
        
        void createTexture(const TextureDescription& description);
        void createTexture(RenderContext& context, TextureDescription const&);

        void lock(RenderContext& context);
        void unlock(RenderContext& context);

        bool supportsMipMaps();
    };
}
