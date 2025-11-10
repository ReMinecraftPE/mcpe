#pragma once

#include <vector>

#include "API_OGL.hpp"
#include "renderer/hal/base/RenderContextBase.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "common/math/Color.hpp"

namespace mce
{
    const GLenum modeMap[] = {
        /*PRIMITIVE_MODE_NONE*/           GL_NONE,
        /*PRIMITIVE_MODE_QUAD_LIST*/      GL_TRIANGLES, // intentionally not using GL_QUADS
        /*PRIMITIVE_MODE_TRIANGLE_LIST*/  GL_TRIANGLES,
        /*PRIMITIVE_MODE_TRIANGLE_STRIP*/ GL_TRIANGLE_STRIP,
        /*PRIMITIVE_MODE_LINE_LIST*/      GL_LINES,
        /*PRIMITIVE_MODE_LINE_STRIP*/     GL_LINE_STRIP
    };
    const GLenum indexType[] = {
        GL_NONE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_NONE,
        GL_UNSIGNED_INT
    };

    class RenderContextOGL : public RenderContextBase
    {
    public:
        struct ActiveTextureUnit
        {
            GLuint m_textureUnit;
            bool m_bIsShaderUniformDirty;

            ActiveTextureUnit()
            {
                m_textureUnit = GL_NONE;
                m_bIsShaderUniformDirty = true;
            }
        };

    private:
        GLuint m_activeBuffer[2]; // indexed by BufferType
        ActiveTextureUnit m_activeTextureUnits[8];

    public:
        GLuint m_activeTexture;
        GLuint m_activeShaderProgram;
        //GLuint m_activeBuffer[2];
        //ActiveTextureUnit m_activeTextureUnits[8];
        std::vector<const GLvoid *> m_activePixels;

    public:
        RenderContextOGL();

    public:
        void draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize);
        void setDepthRange(float nearVal, float farVal);
        void setViewport(unsigned int width, unsigned int height, float nearVal, float farVal);
        void clearFrameBuffer(const Color& color);
        void clearStencilBuffer();
        void clearDepthStencilBuffer();
        void setRenderTarget();
        void swapBuffers();
        void lostContext();
        static int getMaxVertexCount();

        GLuint& getActiveBuffer(BufferType bufferType);
        GLuint getActiveBuffer(BufferType bufferType) const;

        ActiveTextureUnit& getActiveTextureUnit(unsigned int index);
        const ActiveTextureUnit& getActiveTextureUnit(unsigned int index) const;
    };
}
