#pragma once

#include "ShaderProgram.hpp"
#include "renderer/hal/ogl/ShaderOGL.hpp"

namespace mce
{
    class Shader : public ShaderOGL
    {
    public:
        Shader(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
    };
}