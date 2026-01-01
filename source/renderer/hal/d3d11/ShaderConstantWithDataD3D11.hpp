#pragma once

#include "API_D3D11.hpp"

#include "renderer/hal/base/ShaderConstantWithDataBase.hpp"

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithDataD3D11 : public ShaderConstantWithDataBase
    {
    public:
        ShaderConstantWithDataD3D11(ShaderPrimitiveTypes primitiveType)
            : ShaderConstantWithDataBase(primitiveType)
        {
        }
    };
}
