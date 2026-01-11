#include "ConstantBufferContainerD3D9.hpp"
#include "renderer/hal/enums/ShaderStagesBits.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

ConstantBufferContainerD3D9::ConstantBufferContainerD3D9()
    : ConstantBufferContainerBase()
{
}

void ConstantBufferContainerD3D9::bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits)
{
    // shaderBindPoint comes from reflection (e.g., Register c0, c4, etc.)

    if (m_constantBufferBytes->empty())
        return;

    D3DDevice d3dDevice = context.getD3DDevice();

    // Calculate register count (each register is 4 floats = 16 bytes)
    // We round up to ensure we cover the whole buffer.
    unsigned int sizeInBytes = m_constantBufferBytes->size();
    unsigned int vector4Count = (sizeInBytes + 15) / 16;

    const float* pData = (const float*)&m_constantBufferBytes->front();

    if ((shaderStagesBits & SHADER_STAGE_BIT_VERTEX) != 0)
    {
        // Optimization: In D3D9, we might check a shadow state cache here before calling API
        d3dDevice->SetVertexShaderConstantF(shaderBindPoint, pData, vector4Count);
    }

    if ((shaderStagesBits & SHADER_STAGE_BIT_PIXEL) != 0)
    {
        d3dDevice->SetPixelShaderConstantF(shaderBindPoint, pData, vector4Count);
    }
}
