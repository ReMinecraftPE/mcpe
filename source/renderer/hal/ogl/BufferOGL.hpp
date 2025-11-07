#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/BufferBase.hpp"

namespace mce
{
    GLenum glTargetFromBufferType(BufferType bufferType);

    class BufferOGL : public BufferBase
    {
    private:
        GLuint m_bufferName;
        GLenum m_target;

    public:
        BufferOGL();
        ~BufferOGL();

        void release();
        void bindBuffer(RenderContext& context);
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int size, BufferType bufferType, const void *data);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size);
        void updateBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count);
        bool isValid() const { return m_bufferName != GL_NONE; }

        BufferOGL& operator=(BufferOGL&& other);
    };
}