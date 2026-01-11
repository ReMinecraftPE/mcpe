#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/ImmediateBufferBase.hpp"
#include "BufferOGL.hpp"

namespace mce
{
    class ImmediateBufferOGL : public ImmediateBufferBase, public BufferOGL
    {
    public:
        ImmediateBufferOGL();

        void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);
        
        bool isValid() const;
    };
}
