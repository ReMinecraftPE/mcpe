#include "Shader.hpp"

using namespace mce;

Shader::Shader(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : ShaderOGL(vertex, fragment, geometry)
{
}