#pragma once

#include "renderer/hal/base/ShaderConstantWithDataBase.hpp"

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithDataNull : public ShaderConstantWithDataBase
    {
    public:
        ShaderConstantWithDataNull(ShaderPrimitiveTypes primitiveType)
            : ShaderConstantWithDataBase(primitiveType)
        {
        }

        void syncUniform(int value) {};
    };
}
