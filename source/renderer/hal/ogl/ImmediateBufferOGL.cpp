#include <stdexcept>

#include "ImmediateBufferOGL.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "BufferOGL.hpp"

using namespace mce;

ImmediateBufferOGL::ImmediateBufferOGL()
    : ImmediateBufferBase()
{
}

void ImmediateBufferOGL::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    if (gl::supportsImmediateMode())
    {
        BufferBase::createDynamicBuffer(context, stride, data, count, bufferType);
        m_target = glTargetFromBufferType(bufferType);
    }
    else
    {
        BufferOGL::createDynamicBuffer(context, stride, data, count, bufferType);
    }
}

void ImmediateBufferOGL::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    if (gl::supportsImmediateMode())
    {
        // See supportsImmediateMode() definition for explanation on what this is doing
        xglBindBuffer(m_target, GL_NONE);
        GLuint& activeBuffer = context.getActiveBuffer(m_bufferType);
        activeBuffer = GL_NONE;

        // @0.16.1: set *(context + 0x164) to 0
        /*if (m_bufferType == BUFFER_TYPE_INDEX)
            *&context->m_activeTextureUnits[1].m_bIsShaderUniformDirty = 0;*/
    }
    else
    {
        BufferOGL::updateBuffer(context, stride, data, count);
        /* @0.16.1: Unnecessary, Tessellator only did the following to allow for non-temporary buffers
        // More or less doing what the Tessellator did with the VBOs array
        // except we're using one contiguous OGL buffer
        unsigned int nextBufferOffset = m_bufferOffset + (count * stride);
        if (nextBufferOffset > getInternalBufferSize())
        {
            m_bufferOffset = 0;
        }
        BufferOGL::updateBuffer(context, stride, data, count);
        data = (void*)m_bufferOffset;
        m_bufferOffset += count * stride;
        */
    }
}

bool ImmediateBufferOGL::isValid() const
{
    return m_target != GL_NONE;
}
