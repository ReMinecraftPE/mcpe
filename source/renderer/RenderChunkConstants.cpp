#include "RenderChunkConstants.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

RenderChunkConstants::RenderChunkConstants()
{
    CHUNK_ORIGIN = nullptr;
}

void RenderChunkConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("RenderChunkConstants");

    ShaderConstantBase* pChunkOrigin = m_constantBuffer->getUnspecializedShaderConstant("CHUNK_ORIGIN");
    if (pChunkOrigin)
    {
        if (pChunkOrigin->getType() == SHADER_PRIMITIVE_FLOAT3)
        {
            CHUNK_ORIGIN = (ShaderConstantFloat3*)pChunkOrigin;
        }
        else
        {
            CHUNK_ORIGIN = nullptr;
        }
    }
}
