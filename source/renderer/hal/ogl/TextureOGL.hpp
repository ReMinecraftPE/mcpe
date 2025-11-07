#pragma once

#include "renderer/hal/base/TextureBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class Texture;

    class TextureOGL : public TextureBase
    {
    private:
        struct State
        {
            GLuint* m_textureArray;
            GLenum m_textureTarget;
            GLenum m_internalTextureFormat;
            GLenum m_textureFormat;
            GLenum m_textureType;

            State()
            {
                m_textureArray = nullptr;
                m_textureTarget = GL_TEXTURE_2D;
                m_internalTextureFormat = GL_NONE;
                m_textureFormat = GL_NONE;
                m_textureType = GL_NONE;
            }
        };
    private:
        State m_state;

    public:
        TextureOGL();

        void deleteTexture();
        void bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits);
        
        void convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps);

        void subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level);
        void subBuffer(RenderContext& context, const void* pixels);

        void createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level);
        void createTexture(RenderContext& context, TextureDescription const&);

        void lock(RenderContext& context);
        void unlock(RenderContext& context);

        bool supportsMipMaps();
    };
}
