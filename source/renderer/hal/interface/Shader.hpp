#pragma once

#include "ShaderProgram.hpp"
#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER_SHADER(Shader)

namespace mce
{
    class Shader : public MCE_GFX_CLASS_SHADER(Shader)
    {
    public:
        Shader(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
    };
}
