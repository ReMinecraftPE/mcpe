#pragma once

#include "renderer/hal/base/ShaderConstantBase.hpp"

namespace mce
{
    class ShaderConstantNull : public ShaderConstantBase
    {
    public:
        void syncUniform(int) {}
    };
}
