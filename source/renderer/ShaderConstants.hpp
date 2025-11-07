#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"
#include "common/math/Color.hpp"

// couldn't find that these were even initialized
static Color currentShaderColor;
static Color currentShaderDarkColor;

namespace mce
{
    class ShaderConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat4* CURRENT_COLOR;
        ShaderConstantFloat4* DARKEN;

    public:
        ShaderConstants();

        void setShaderColor(const Color& shaderColor);
        void setShaderColors( const Color& shaderColor, const Color& shaderDarkenColor);

        void init();
    };
}