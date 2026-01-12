#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"

namespace mce
{
    class WeatherConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat4 *POSITION_OFFSET;
        ShaderConstantFloat4 *VELOCITY;
        ShaderConstantFloat4 *ALPHA;
        ShaderConstantFloat4 *VIEW_POSITION;
        ShaderConstantFloat4 *SIZE_SCALE;
        ShaderConstantFloat4 *FORWARD;
        ShaderConstantFloat4 *UV_INFO;
        ShaderConstantFloat4 *PARTICLE_BOX;

    public:
        WeatherConstants();

        void init();
    };
}
