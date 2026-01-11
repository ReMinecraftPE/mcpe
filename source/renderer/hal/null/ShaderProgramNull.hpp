#pragma once

#include "renderer/hal/base/ShaderProgramBase.hpp"

namespace mce
{
    class ShaderProgramNull : public ShaderProgramBase
    {
    public:
        ShaderProgramNull(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath);
    };
}
