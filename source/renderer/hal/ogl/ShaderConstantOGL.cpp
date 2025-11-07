#include "ShaderConstantOGL.hpp"

using namespace mce;

void ShaderConstantOGL::syncUniform(int value)
{
    m_dirty = false;
}