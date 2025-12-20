#include "ShaderConstants.hpp"
#include "GlobalConstantBufferManager.hpp"
#include "RenderContextImmediate.hpp"

using namespace mce;

Color currentShaderColor = Color::WHITE;
Color currentShaderDarkColor = Color::WHITE;

ShaderConstants::ShaderConstants()
{
    CURRENT_COLOR = nullptr;
    DARKEN = nullptr;
    TEXTURE_DIMENSIONS = nullptr;
    HUD_OPACITY = nullptr;
    UV_TRANSFORM = nullptr;
}

void ShaderConstants::setShaderColor(const Color& shaderColor)
{
#ifdef FEATURE_GFX_SHADERS
    CURRENT_COLOR->setData(&shaderColor);
    sync();
#else
    RenderContext& renderContext = RenderContextImmediate::get();
    renderContext.setCurrentColor(shaderColor);
#endif
}

void ShaderConstants::setShaderColors(const Color& shaderColor, const Color& shaderDarkenColor)
{
#ifdef FEATURE_GFX_SHADERS
    CURRENT_COLOR->setData(&shaderColor);
    DARKEN->setData(&shaderDarkenColor);

    sync();
#else
    RenderContext& renderContext = RenderContextImmediate::get();
    renderContext.setCurrentColor(shaderColor * shaderDarkenColor);
#endif
}

void ShaderConstants::init()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    m_constantBuffer = bufferManager.findConstantBufferContainer("ShaderConstants");

    ShaderConstantBase* pCurrentColor = m_constantBuffer->getUnspecializedShaderConstant("CURRENT_COLOR");
    if (pCurrentColor)
    {
        if (pCurrentColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            CURRENT_COLOR = (ShaderConstantFloat4*)pCurrentColor;
        }
        else
        {
            CURRENT_COLOR = nullptr;
        }
    }

    ShaderConstantBase* pDarkenColor = m_constantBuffer->getUnspecializedShaderConstant("DARKEN");
    if (pDarkenColor)
    {
        if (pDarkenColor->getType() == SHADER_PRIMITIVE_FLOAT4)
        {
            DARKEN = (ShaderConstantFloat4*)pDarkenColor;
        }
        else
        {
            DARKEN = nullptr;
        }
    }


    ShaderConstantBase* pTextureDimensions = m_constantBuffer->getUnspecializedShaderConstant("TEXTURE_DIMENSIONS");
    if (pTextureDimensions)
    {
        if (pTextureDimensions->getType() == SHADER_PRIMITIVE_FLOAT3)
        {
            TEXTURE_DIMENSIONS = (ShaderConstantFloat3*)pTextureDimensions;
        }
        else
        {
            TEXTURE_DIMENSIONS = nullptr;
        }
    }


    ShaderConstantBase* pHudOpacity = m_constantBuffer->getUnspecializedShaderConstant("HUD_OPACITY");
    if (pHudOpacity)
    {
        if (pHudOpacity->getType() == SHADER_PRIMITIVE_FLOAT1)
        {
            HUD_OPACITY = (ShaderConstantFloat1*)pHudOpacity;
        }
        else
        {
            HUD_OPACITY = nullptr;
        }
    }


    ShaderConstantBase* pUVTransform = m_constantBuffer->getUnspecializedShaderConstant("UV_TRANSFORM");
    if (pUVTransform)
    {
        if (pUVTransform->getType() == SHADER_PRIMITIVE_MATRIX4x4)
        {
            UV_TRANSFORM = (ShaderConstantMatrix4x4*)pUVTransform;
        }
        else
        {
            UV_TRANSFORM = nullptr;
        }
    }
}