#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/ImmediateBufferBase.hpp"
#include "BufferD3D9.hpp"

namespace mce
{
    class ImmediateBufferD3D9 : public ImmediateBufferBase, public BufferD3D9
    {
    public:
        ImmediateBufferD3D9();

        void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);

        bool isValid() const;
    };
}