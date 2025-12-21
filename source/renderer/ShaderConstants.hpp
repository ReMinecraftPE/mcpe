#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"
#include "common/math/Color.hpp"

extern Color currentShaderColor;
extern Color currentShaderDarkColor;

namespace mce
{
    class ShaderConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat4* CURRENT_COLOR;
        ShaderConstantFloat4* DARKEN;
        ShaderConstantFloat3* TEXTURE_DIMENSIONS;
        ShaderConstantFloat1* HUD_OPACITY;
        ShaderConstantMatrix4x4* UV_TRANSFORM;


    public:
        ShaderConstants();

        void setShaderColor(const Color& shaderColor);
        void setShaderColors( const Color& shaderColor, const Color& shaderDarkenColor);

        void init();
    };
}