#include <stdexcept>
#include "RenderContextOGL.hpp"

using namespace mce;

RenderContextOGL::RenderContextOGL()
    : RenderContextBase()
{
    m_activeTexture = GL_NONE;
    m_activeShaderProgram = GL_NONE;
}

void RenderContextOGL::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
    xglDrawArrays(modeMap[primitiveMode], startOffset, count);
}

void RenderContextOGL::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
    glDrawElements(modeMap[primitiveMode], count, indexType[indexSize], nullptr);
}

void RenderContextOGL::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
    glDrawElements(modeMap[primitiveMode], count, indexType[indexSize], (const GLvoid*)(startOffset * indexSize));
}

void RenderContextOGL::setDepthRange(float nearVal, float farVal)
{
    glDepthRange(nearVal, farVal);
}

void RenderContextOGL::setViewport(unsigned int width, unsigned int height, float nearVal, float farVal)
{
    glViewport(0, 0, width, height);
    setDepthRange(nearVal, farVal);
}

void RenderContextOGL::clearFrameBuffer(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContextOGL::clearStencilBuffer()
{
    glStencilMask(0xFFFFFFFF);
    glClear(GL_STENCIL_BUFFER_BIT);
}

void RenderContextOGL::clearDepthStencilBuffer()
{
    glStencilMask(0xFFFFFFFF);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderContextOGL::setRenderTarget()
{
}

void RenderContextOGL::swapBuffers()
{
}

void RenderContextOGL::lostContext()
{
    m_activeTexture = GL_NONE;
    m_activeBuffer[0] = GL_NONE;
    m_activeBuffer[1] = GL_NONE;
}

int RenderContextOGL::getMaxVertexCount()
{
    return gl::getMaxVertexCount();
}

GLuint& RenderContextOGL::getActiveBuffer(BufferType bufferType)
{
    if (bufferType > BUFFER_TYPES_MAX)
        throw std::out_of_range("m_activeBuffer[]");
    
    return m_activeBuffer[bufferType];
}

GLuint RenderContextOGL::getActiveBuffer(BufferType bufferType) const
{
    if (bufferType > BUFFER_TYPES_MAX)
        throw std::out_of_range("m_activeBuffer[]");
    
    return m_activeBuffer[bufferType];
}

RenderContextOGL::ActiveTextureUnit& RenderContextOGL::getActiveTextureUnit(unsigned int index)
{
    if (index >= 8)
        throw std::out_of_range("m_activeTextureUnits[]");
    
    return m_activeTextureUnits[index];
}

const RenderContextOGL::ActiveTextureUnit& RenderContextOGL::getActiveTextureUnit(unsigned int index) const
{
    if (index >= 8)
        throw std::out_of_range("m_activeTextureUnits[]");
    
    return m_activeTextureUnits[index];
}