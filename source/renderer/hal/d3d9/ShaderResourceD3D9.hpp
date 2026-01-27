#pragma once

#include "renderer/hal/enums/ShaderType.hpp"
#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    class ShaderResourceD3D9
    {
    public:
        ShaderConstantD3D9* m_pShaderConstant;
        ConstantBufferContainer* m_pConstantBufferContainer;
        unsigned int m_shaderBindPoints[SHADER_TYPES_COUNT];
        unsigned int m_shaderStagesBits;
        std::string m_name;

        ShaderResourceD3D9(const std::string& name);

        void bind(RenderContext& context, ShaderType shaderType, bool forceBind);
    };
}