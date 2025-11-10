#include "Shader.hpp"

using namespace mce;

Shader::Shader(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : MCE_GFX_CLASS(Shader)(vertex, fragment, geometry)
{
}