#include "Shader.hpp"

using namespace mce;

Shader::Shader(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : MCE_GFX_CLASS_SHADER(Shader)(vertex, fragment, geometry)
{
}