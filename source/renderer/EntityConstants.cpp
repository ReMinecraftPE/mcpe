#include "EntityConstants.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

EntityConstants::EntityConstants()
{
    OVERLAY_COLOR = nullptr;
    TILE_LIGHT_COLOR = nullptr;
    CHANGE_COLOR = nullptr;
    // only these were initialized for some reason.
    // I guess everything else can go fuck itself
}

void EntityConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("EntityConstants");

    ShaderConstantBase* pOverlayColor = m_constantBuffer->getUnspecializedShaderConstant("OVERLAY_COLOR");
    if (pOverlayColor)
    {
        if (pOverlayColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            OVERLAY_COLOR = (ShaderConstantFloat4*)pOverlayColor;
        }
        else
        {
            OVERLAY_COLOR = nullptr;
        }
    }

    ShaderConstantBase* pTileLightColor = m_constantBuffer->getUnspecializedShaderConstant("TILE_LIGHT_COLOR");
    if (pTileLightColor)
    {
        if (pTileLightColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            TILE_LIGHT_COLOR = (ShaderConstantFloat4*)pTileLightColor;
        }
        else
        {
            TILE_LIGHT_COLOR = nullptr;
        }
    }

    ShaderConstantBase* pChangeColor = m_constantBuffer->getUnspecializedShaderConstant("CHANGE_COLOR");
    if (pChangeColor)
    {
        if (pChangeColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            CHANGE_COLOR = (ShaderConstantFloat4*)pChangeColor;
        }
        else
        {
            CHANGE_COLOR = nullptr;
        }
    }

    ShaderConstantBase* pUvAnim = m_constantBuffer->getUnspecializedShaderConstant("UV_ANIM");
    if (pUvAnim)
    {
        if (pUvAnim->getType() == SHADER_PRIMITIVE_FLOAT2)
        {
            UV_ANIM = (ShaderConstantFloat2*)pUvAnim;
        }
        else
        {
            UV_ANIM = nullptr;
        }
    }

    ShaderConstantBase* pUvOffset = m_constantBuffer->getUnspecializedShaderConstant("UV_OFFSET");
    if (pUvOffset)
    {
        if (pUvOffset->getType() == SHADER_PRIMITIVE_FLOAT2)
        {
            UV_OFFSET = (ShaderConstantFloat2*)pUvOffset;
        }
        else
        {
            UV_OFFSET = nullptr;
        }
    }

    ShaderConstantBase* pUvRotation = m_constantBuffer->getUnspecializedShaderConstant("UV_ROTATION");
    if (pUvRotation)
    {
        if (pUvRotation->getType() == SHADER_PRIMITIVE_FLOAT2)
        {
            UV_ROTATION = (ShaderConstantFloat2*)pUvRotation;
        }
        else
        {
            UV_ROTATION = nullptr;
        }
    }

    ShaderConstantBase* pGlintUvScale = m_constantBuffer->getUnspecializedShaderConstant("GLINT_UV_SCALE");
    if (pGlintUvScale)
    {
        if (pGlintUvScale->getType() == SHADER_PRIMITIVE_FLOAT2)
        {
            GLINT_UV_SCALE = (ShaderConstantFloat2*)pGlintUvScale;
        }
        else
        {
            GLINT_UV_SCALE = nullptr;
        }
    }
}