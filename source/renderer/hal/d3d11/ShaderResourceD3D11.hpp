#pragma once

#include "renderer/hal/enums/ShaderType.hpp"
#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    class ShaderResourceD3D11
    {
    public:
        ConstantBufferContainer* m_pConstantBufferContainer;
        unsigned int m_shaderBindPoints[SHADER_TYPES_COUNT];
        unsigned int m_shaderStagesBits;

        ShaderResourceD3D11();
    };
}
