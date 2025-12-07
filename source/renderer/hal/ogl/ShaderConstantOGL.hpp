#pragma once

#include "renderer/hal/base/ShaderConstantBase.hpp"

namespace mce
{
    class ShaderConstantOGL : public ShaderConstantBase
    {
    public:
        virtual void syncUniform(int value);
    };
}