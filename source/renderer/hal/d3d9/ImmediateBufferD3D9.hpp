#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/ImmediateBufferBase.hpp"
#include "BufferD3D9.hpp"

namespace mce
{
    class ImmediateBufferD3D9 : public ImmediateBufferBase
    {
    protected:
        BufferD3D9 m_buffer1;
        BufferD3D9 m_buffer2;
        BufferD3D9* m_pBuffer;

    protected:
        void _move(ImmediateBufferD3D9& other);

    public:
        ImmediateBufferD3D9();
        MC_CTOR_MOVE(ImmediateBufferD3D9);

    protected:
        void _swapBuffers();

    public:
        void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);
        bool isValid() const;

        MC_FUNC_MOVE(ImmediateBufferD3D9);
    };
}