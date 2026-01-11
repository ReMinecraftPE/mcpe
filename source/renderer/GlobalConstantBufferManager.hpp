#pragma once

#include <vector>

#include "common/utility/Singleton.hpp"
#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    class GlobalConstantBufferManager : public Singleton<GlobalConstantBufferManager>
    {
    public:
        std::vector<ConstantBufferContainer> m_constantBufferContainers;

    public:
        GlobalConstantBufferManager();

    public:
        void refreshWorldConstants();
        void allocateAndSetupConstantBuffersFromMetadata(RenderContext& context);
        ConstantBufferContainer* findConstantBufferContainer(const std::string& bufferName);
    };
}
