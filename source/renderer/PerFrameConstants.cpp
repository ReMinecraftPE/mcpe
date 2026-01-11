#include "PerFrameConstants.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

PerFrameConstants::PerFrameConstants()
{
    VIEW_DIRECTION = nullptr;
    TIME = nullptr;
    VIEW_POS = nullptr;
    FAR_CHUNKS_DISTANCE = nullptr;
    FOG_COLOR = nullptr;
    FOG_CONTROL = nullptr;
    RENDER_DISTANCE = nullptr;
}

void PerFrameConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("PerFrameConstants");

    ShaderConstantBase* pViewDirection = m_constantBuffer->getUnspecializedShaderConstant("VIEW_DIRECTION");
    if (pViewDirection)
    {
        if (pViewDirection->getType() == SHADER_PRIMITIVE_FLOAT3)
        {
            VIEW_DIRECTION = (ShaderConstantFloat3*)pViewDirection;
        }
        else
        {
            VIEW_DIRECTION = nullptr;
        }
    }

    ShaderConstantBase* pTime = m_constantBuffer->getUnspecializedShaderConstant("TIME");
    if (pTime)
    {
        if (pTime->getType() == SHADER_PRIMITIVE_FLOAT1)
        {
            TIME = (ShaderConstantFloat1*)pTime;
        }
        else
        {
            TIME = nullptr;
        }
    }

    ShaderConstantBase* pViewPos = m_constantBuffer->getUnspecializedShaderConstant("VIEW_POS");
    if (pViewPos)
    {
        if (pViewPos->getType() == SHADER_PRIMITIVE_FLOAT3)
        {
            VIEW_POS = (ShaderConstantFloat3*)pViewPos;
        }
        else
        {
            VIEW_POS = nullptr;
        }
    }

    ShaderConstantBase* pFarChunksDistance = m_constantBuffer->getUnspecializedShaderConstant("FAR_CHUNKS_DISTANCE");
    if (pFarChunksDistance)
    {
        if (pFarChunksDistance->getType() == SHADER_PRIMITIVE_FLOAT1)
        {
            FAR_CHUNKS_DISTANCE = (ShaderConstantFloat1*)pFarChunksDistance;
        }
        else
        {
            FAR_CHUNKS_DISTANCE = nullptr;
        }
    }

    ShaderConstantBase* pFogColor = m_constantBuffer->getUnspecializedShaderConstant("FOG_COLOR");
    if (pFogColor)
    {
        if (pFogColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            FOG_COLOR = (ShaderConstantFloat4*)pFogColor;
        }
        else
        {
            FOG_COLOR = nullptr;
        }
    }

    ShaderConstantBase* pFogControl = m_constantBuffer->getUnspecializedShaderConstant("FOG_CONTROL");
    if (pFogControl)
    {
        if (pFogControl->getType() == SHADER_PRIMITIVE_FLOAT2)
        {
            FOG_CONTROL = (ShaderConstantFloat2*)pFogControl;
        }
        else
        {
            FOG_CONTROL = nullptr;
        }
    }

    ShaderConstantBase* pRenderDistance = m_constantBuffer->getUnspecializedShaderConstant("RENDER_DISTANCE");
    if (pRenderDistance)
    {
        if (pRenderDistance->getType() == SHADER_PRIMITIVE_FLOAT1)
        {
            RENDER_DISTANCE = (ShaderConstantFloat1*)pRenderDistance;
        }
        else
        {
            RENDER_DISTANCE = nullptr;
        }
    }
}
