#pragma once

#include "API_D3D9.hpp"

#include "renderer/hal/base/ShaderConstantWithDataBase.hpp"

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithDataD3D9 : public ShaderConstantWithDataBase
    {
    public:
        ShaderConstantWithDataD3D9(ShaderPrimitiveTypes primitiveType)
            : ShaderConstantWithDataBase(primitiveType)
        {
        }

        void syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location) override;
    };
}
