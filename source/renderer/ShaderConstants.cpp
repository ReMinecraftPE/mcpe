#include "GlobalConstantBufferManager.hpp"
#include "RenderContextImmediate.hpp"
#include "ShaderConstants.hpp"

using namespace mce;

ShaderConstants::ShaderConstants()
{
    CURRENT_COLOR = nullptr;
    DARKEN = nullptr;
}

void ShaderConstants::setShaderColor(const Color& shaderColor)
{
    CURRENT_COLOR->m_data = (uint8_t*)&shaderColor;
    CURRENT_COLOR->m_dirty = true;
    
    m_constantBuffer->sync(RenderContextImmediate::get());
}

void ShaderConstants::setShaderColors(const Color& shaderColor, const Color& shaderDarkenColor)
{
    Color* pCurrentColor = (Color*)CURRENT_COLOR->m_data;
    *pCurrentColor = shaderColor;
    CURRENT_COLOR->m_dirty = true;

    Color* pDarkenColor = (Color*)DARKEN->m_data;
    *pDarkenColor = shaderDarkenColor;
    DARKEN->m_dirty = true;

    m_constantBuffer->sync(RenderContextImmediate::get());
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