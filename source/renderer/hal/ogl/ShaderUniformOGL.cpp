#include "ShaderUniformOGL.hpp"

using namespace mce;

ShaderUniformOGL::ShaderUniformOGL(const std::string& name, int location, int elements, ShaderPrimitiveTypes shaderPrimitiveType)
    : ShaderResourceOGL(name, location, elements, shaderPrimitiveType)
{
    m_pShaderConstant = nullptr;
    m_pConstantBufferContainer = nullptr;
}

void ShaderUniformOGL::bind(bool forceBind)
{
    if (forceBind || m_pShaderConstant->isDirty())
    {
        m_pShaderConstant->syncUniform(m_location);
    }
}
