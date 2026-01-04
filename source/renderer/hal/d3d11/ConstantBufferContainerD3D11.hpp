#pragma once

#include "renderer/hal/base/ConstantBufferContainerBase.hpp"

namespace mce
{
    class ConstantBufferContainerD3D11 : public ConstantBufferContainerBase
    {
    public:
        ComInterface<ID3D11Buffer> m_buffer;

    public:
        ConstantBufferContainerD3D11();

    public:
        void sync(RenderContext& context);
        void allocateRenderContextBuffer(RenderContext& context);
        void bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits);
        void* map(RenderContext& context);
        void unmap(RenderContext& context);
    };
}