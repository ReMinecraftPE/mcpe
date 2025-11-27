#include "GlobalConstantBufferManager.hpp"
#include "ShaderConstants.hpp"

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
#if FEATURE_GFX_SHADERS
    Color* pCurrentColor = (Color*)CURRENT_COLOR->m_data;
    *pCurrentColor = shaderColor;
    CURRENT_COLOR->m_dirty = true;
    
    sync();
#else
    // @TODO: abstract
    glColor4f(shaderColor.r, shaderColor.g, shaderColor.b, shaderColor.a);
#endif
}

void ShaderConstants::setShaderColors(const Color& shaderColor, const Color& shaderDarkenColor)
{
#if FEATURE_GFX_SHADERS
    Color* pCurrentColor = (Color*)CURRENT_COLOR->m_data;
    *pCurrentColor = shaderColor;
    CURRENT_COLOR->m_dirty = true;

    Color* pDarkenColor = (Color*)DARKEN->m_data;
    *pDarkenColor = shaderDarkenColor;
    DARKEN->m_dirty = true;

    sync();
#else
    Color color = shaderColor * shaderDarkenColor;
    // @TODO: abstract
    glColor4f(color.r, color.g, color.b, color.a);
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