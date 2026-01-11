#pragma once

#include "renderer/hal/base/ShaderConstantBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class ShaderConstantD3D9 : public ShaderConstantBase
    {
    public:
        virtual void syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location);
    };
}