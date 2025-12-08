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
}

void ShaderConstants::setShaderColor(const Color& shaderColor)
{
#ifdef FEATURE_GFX_SHADERS
    float* pCurrentColor = (float*)CURRENT_COLOR->m_data;
    pCurrentColor[0] = shaderColor.r;
    pCurrentColor[1] = shaderColor.g;
    pCurrentColor[2] = shaderColor.b;
    pCurrentColor[3] = shaderColor.a;
    CURRENT_COLOR->m_dirty = true;
    
    sync();
#else
    RenderContext& renderContext = RenderContextImmediate::get();
    renderContext.setCurrentColor(shaderColor);
#endif
}

void ShaderConstants::setShaderColors(const Color& shaderColor, const Color& shaderDarkenColor)
{
#ifdef FEATURE_GFX_SHADERS
    float* pCurrentColor = (float*)CURRENT_COLOR->m_data;
    pCurrentColor[0] = shaderColor.r;
    pCurrentColor[1] = shaderColor.g;
    pCurrentColor[2] = shaderColor.b;
    pCurrentColor[3] = shaderColor.a;
    CURRENT_COLOR->m_dirty = true;

    float* pDarkenColor = (float*)DARKEN->m_data;
    pDarkenColor[0] = shaderDarkenColor.r;
    pDarkenColor[1] = shaderDarkenColor.g;
    pDarkenColor[2] = shaderDarkenColor.b;
    pDarkenColor[3] = shaderDarkenColor.a;
    DARKEN->m_dirty = true;

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
}