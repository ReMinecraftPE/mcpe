#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/BufferBase.hpp"

namespace mce
{
    GLenum glTargetFromBufferType(BufferType bufferType);

    class BufferOGL : public BufferBase
    {
    protected:
        GLuint m_bufferName;
        GLenum m_target;
        GLenum m_usage;

    public:
        BufferOGL();
		MC_CTOR_MOVE(BufferOGL);
        ~BufferOGL();

    protected:
        void _createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);

    public:
		void _move(BufferOGL& other);
        void releaseBuffer();
        void bindBuffer(RenderContext& context);
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);
        bool isValid() const { return m_bufferName != GL_NONE; }

        MC_FUNC_MOVE(BufferOGL);
	};
}
