#pragma once

#include "renderer/hal/base/ShaderBase.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"

namespace mce
{
    class ShaderNull : public ShaderBase
    {
    public:
        ShaderNull(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
    };
}
