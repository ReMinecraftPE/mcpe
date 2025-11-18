#pragma once

#include <string>

#include "renderer/hal/enums/ShaderType.hpp"
#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER_SHADER(ShaderProgram)

namespace mce
{
    class ShaderProgram : public MCE_GFX_CLASS_SHADER(ShaderProgram)
    {
    public:
        ShaderProgram(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath);
    };
}