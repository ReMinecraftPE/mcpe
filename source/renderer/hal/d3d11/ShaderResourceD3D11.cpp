#include "ShaderResourceD3D11.hpp"

using namespace mce;

ShaderResourceD3D11::ShaderResourceD3D11()
{
    m_pConstantBufferContainer = nullptr;
    m_shaderStagesBits = 0x0;

    for (int i = 0; i < SHADER_TYPES_COUNT; i++)
        m_shaderBindPoints[i] = 0;
}