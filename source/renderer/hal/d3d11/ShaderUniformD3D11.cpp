#include "ShaderUniformOGL.hpp"

using namespace mce;

ShaderUniformOGL::ShaderUniformOGL(const std::string& name, int location, int elements, ShaderPrimitiveTypes shaderPrimitiveType)
    : ShaderResourceOGL(name, location, elements, shaderPrimitiveType)
{
    m_shaderConstant = nullptr;
    m_constantBufferContainer = nullptr;
}

void ShaderUniformOGL::bind(bool forceBind)
{
    if (forceBind || m_shaderConstant->isDirty())
    {
        m_shaderConstant->syncUniform(m_location);
    }
}
