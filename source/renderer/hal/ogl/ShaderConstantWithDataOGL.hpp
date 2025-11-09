#pragma once

#include "API_OGL.hpp"

#include "renderer/hal/base/ShaderConstantWithDataBase.hpp"

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithDataOGL : public ShaderConstantWithDataBase
    {
    public:
        ShaderConstantWithDataOGL(ShaderPrimitiveTypes primitiveType)
            : ShaderConstantWithDataBase(primitiveType)
        {
        }

        void syncUniform(int value);
    };
}
