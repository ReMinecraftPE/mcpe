#include <stdexcept>

#include "ImmediateBufferOGL.hpp"
#include "BufferOGL.hpp"

using namespace mce;

ImmediateBufferOGL::ImmediateBufferOGL()
    : ImmediateBufferBase()
{
    m_target = GL_NONE;
}

void ImmediateBufferOGL::createDynamicBuffer(RenderContext& context, unsigned int bufferSize, BufferType bufferType, void const* data)
{
    m_bufferType = bufferType;
    m_target = glTargetFromBufferType(bufferType);
}

void ImmediateBufferOGL::updateBuffer(RenderContext& context, unsigned int itemSize, void const* data, unsigned int bufferSize)
{
    xglBindBuffer(m_target, GL_NONE);
    GLuint& activeBuffer = context.getActiveBuffer(m_bufferType);
    activeBuffer = GL_NONE;
}

bool ImmediateBufferOGL::isValid() const
{
    return m_target != GL_NONE;
}