#pragma once

#include "renderer/hal/base/ConstantBufferContainerBase.hpp"

namespace mce
{
    class ConstantBufferContainerD3D9 : public ConstantBufferContainerBase
    {
    public:
        ConstantBufferContainerD3D9();

    public:
        void bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits);
    };
}