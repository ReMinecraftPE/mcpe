#pragma once

#include "API_D3D11.hpp"
#include "renderer/hal/base/ImmediateBufferBase.hpp"
#include "BufferD3D11.hpp"

namespace mce
{
    class ImmediateBufferD3D11 : public ImmediateBufferBase, BufferD3D11
    {
    public:
        ImmediateBufferD3D11();

    public:
        void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);

        bool isValid() const;
    };
}
