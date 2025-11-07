#pragma once

#include <string>

#include "renderer/hal/enums/ShaderType.hpp"
#include "renderer/hal/ogl/ShaderProgramOGL.hpp"

namespace mce
{
    class ShaderProgram : public ShaderProgramOGL
    {
    public:
        ShaderProgram(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath);
    };
}