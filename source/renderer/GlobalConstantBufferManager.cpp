#include <typeinfo>
#include "GlobalConstantBuffers.hpp"
#include "ShaderConstants.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

GlobalConstantBufferManager::GlobalConstantBufferManager()
{
}

void GlobalConstantBufferManager::refreshWorldConstants()
{
    GlobalConstantBuffers& buffers = GlobalConstantBuffers::getInstance();

    buffers.m_worldConstants.refreshWorldConstants();
    buffers.m_shaderConstants.setShaderColors(currentShaderColor, currentShaderDarkColor);
}

void GlobalConstantBufferManager::allocateAndSetupConstantBuffersFromMetadata(RenderContext& context)
{
    for (std::vector<ConstantBufferContainer>::iterator it = m_constantBufferContainers.begin(); it != m_constantBufferContainers.end(); it++)
    {
        it->allocateRenderContextBuffer(context);
    }
}

ConstantBufferContainer* GlobalConstantBufferManager::findConstantBufferContainer(const std::string& bufferName)
{
    for (std::vector<ConstantBufferContainer>::iterator it = m_constantBufferContainers.begin(); it != m_constantBufferContainers.end(); it++)
    {
        if (it->getConstantBufferName() == bufferName)
            return &*it; // solid
    }

    LOG_E("Couldn't find the constant buffer named: %s was it properly reflected in the uniforms metadata file?", bufferName);
    throw std::bad_cast();
}