#pragma once

#include <string>
#include "renderer/hal/enums/ShaderType.hpp"

namespace mce
{
    class ShaderProgramBase
    {
    protected:
        bool m_bValid;
    public:
        const std::string m_header;
        const std::string m_shaderPath;
        const ShaderType m_shaderType;

        ShaderProgramBase(const std::string& header, const std::string& shaderPath, ShaderType shaderType);

        bool isValid() const { return m_bValid; }
    };
}