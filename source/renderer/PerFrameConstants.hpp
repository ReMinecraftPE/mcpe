#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"

namespace mce
{
    class PerFrameConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat3 *VIEW_DIRECTION;
        ShaderConstantFloat1 *TIME;
        ShaderConstantFloat3 *VIEW_POS;
        ShaderConstantFloat1 *FAR_CHUNKS_DISTANCE;
        ShaderConstantFloat4 *FOG_COLOR;
        ShaderConstantFloat2 *FOG_CONTROL;
        ShaderConstantFloat1 *RENDER_DISTANCE;

    public:
        PerFrameConstants();

        void init();
    };
}
