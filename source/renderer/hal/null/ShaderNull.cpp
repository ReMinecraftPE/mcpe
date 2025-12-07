#include "ShaderNull.hpp"

using namespace mce;

ShaderNull::ShaderNull(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : ShaderBase(vertex, fragment, geometry)
{
}
