#pragma once

#include <vector>
#include <map>
#include <string>
#include "renderer/hal/interface/Shader.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"

namespace mce
{
    class ShaderGroupBase
    {
    public:
        std::vector<Shader*> m_shaders;
        std::map<std::string, ShaderProgram*> m_programs;

    public:
        ShaderGroupBase();
    };
}