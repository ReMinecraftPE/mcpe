#include "WeatherConstants.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

WeatherConstants::WeatherConstants()
{
    POSITION_OFFSET = nullptr;
    VELOCITY = nullptr;
    ALPHA = nullptr;
    VIEW_POSITION = nullptr;
    SIZE_SCALE = nullptr;
    FORWARD = nullptr;
    UV_INFO = nullptr;
    PARTICLE_BOX = nullptr;
}

void WeatherConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("WeatherConstants");

    ShaderConstantBase* pPositionOffset = m_constantBuffer->getUnspecializedShaderConstant("POSITION_OFFSET");
    if (pPositionOffset)
    {
        if (pPositionOffset->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            POSITION_OFFSET = (ShaderConstantFloat4*)pPositionOffset;
        }
        else
        {
            POSITION_OFFSET = nullptr;
        }
    }

    ShaderConstantBase* pVelocity = m_constantBuffer->getUnspecializedShaderConstant("VELOCITY");
    if (pVelocity)
    {
        if (pVelocity->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            VELOCITY = (ShaderConstantFloat4*)pVelocity;
        }
        else
        {
            VELOCITY = nullptr;
        }
    }

    ShaderConstantBase* pAlpha = m_constantBuffer->getUnspecializedShaderConstant("ALPHA");
    if (pAlpha)
    {
        if (pAlpha->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            ALPHA = (ShaderConstantFloat4*)pAlpha;
        }
        else
        {
            ALPHA = nullptr;
        }
    }

    ShaderConstantBase* pViewPosition = m_constantBuffer->getUnspecializedShaderConstant("VIEW_POSITION");
    if (pViewPosition)
    {
        if (pViewPosition->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            VIEW_POSITION = (ShaderConstantFloat4*)pViewPosition;
        }
        else
        {
            VIEW_POSITION = nullptr;
        }
    }

    ShaderConstantBase* pSizeScale = m_constantBuffer->getUnspecializedShaderConstant("SIZE_SCALE");
    if (pSizeScale)
    {
        if (pSizeScale->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            SIZE_SCALE = (ShaderConstantFloat4*)pSizeScale;
        }
        else
        {
            SIZE_SCALE = nullptr;
        }
    }

    ShaderConstantBase* pForward = m_constantBuffer->getUnspecializedShaderConstant("FORWARD");
    if (pForward)
    {
        if (pForward->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            FORWARD = (ShaderConstantFloat4*)pForward;
        }
        else
        {
            FORWARD = nullptr;
        }
    }

    ShaderConstantBase* pUvInfo = m_constantBuffer->getUnspecializedShaderConstant("UV_INFO");
    if (pUvInfo)
    {
        if (pUvInfo->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            UV_INFO = (ShaderConstantFloat4*)pUvInfo;
        }
        else
        {
            UV_INFO = nullptr;
        }
    }

    ShaderConstantBase* pParticleBox = m_constantBuffer->getUnspecializedShaderConstant("PARTICLE_BOX");
    if (pParticleBox)
    {
        if (pParticleBox->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            PARTICLE_BOX = (ShaderConstantFloat4*)pParticleBox;
        }
        else
        {
            PARTICLE_BOX = nullptr;
        }
    }
}
