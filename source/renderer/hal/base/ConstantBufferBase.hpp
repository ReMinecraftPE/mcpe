#pragma once

#include "renderer/hal/interface/ConstantBufferContainer.hpp"
#include "renderer/hal/ShaderStage.hpp"

namespace mce
{
    class ConstantBufferBase
    {
    public:
        std::vector<unsigned int> m_bindingSlots;
        ShaderStage m_shaderStage;
        ConstantBufferContainer *m_constantBufferContainer;

        void createConstantBuffer(RenderContext &context, unsigned int count);
        unsigned int& getInternalMemory();
    };
}