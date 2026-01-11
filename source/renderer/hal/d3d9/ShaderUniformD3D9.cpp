#include "ShaderUniformD3D9.hpp"

using namespace mce;

ShaderUniformD3D9::ShaderUniformD3D9()
{
    m_pShaderConstant = nullptr;
}

void ShaderUniformD3D9::bind(bool forceBind)
{
    if (forceBind || m_pShaderConstant->isDirty())
    {
        m_pShaderConstant->syncUniform(m_location);
    }
}
