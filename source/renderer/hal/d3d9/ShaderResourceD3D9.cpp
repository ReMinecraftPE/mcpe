#include "ShaderResourceD3D9.hpp"

using namespace mce;

ShaderResourceD3D9::ShaderResourceD3D9(const std::string& name)
    : m_name(name)
{
    m_pShaderConstant = nullptr;
    m_pConstantBufferContainer = nullptr;
    m_shaderStagesBits = 0x0;

    for (int i = 0; i < SHADER_TYPES_COUNT; i++)
        m_shaderBindPoints[i] = 0;
}

void ShaderResourceD3D9::bind(RenderContext& context, ShaderType shaderType, bool forceBind)
{
    if (forceBind || m_pShaderConstant->isDirty())
    {
        m_pShaderConstant->syncConstant(context, shaderType, m_shaderBindPoints[shaderType]);
    }
}