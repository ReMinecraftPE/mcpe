#pragma once

#include "hal/interface/ConstantBufferConstants.hpp"
#include "hal/interface/ShaderConstantWithData.hpp"

namespace mce
{
    class EntityConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat4 *OVERLAY_COLOR;
        ShaderConstantFloat4 *TILE_LIGHT_COLOR;
        ShaderConstantFloat4 *CHANGE_COLOR;
        ShaderConstantFloat2 *UV_ANIM;
        ShaderConstantFloat2 *UV_OFFSET;
        ShaderConstantFloat2 *UV_ROTATION;
        ShaderConstantFloat2 *GLINT_UV_SCALE;

    public:
        EntityConstants();

        void init();
    };
}