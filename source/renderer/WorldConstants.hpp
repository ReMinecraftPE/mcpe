#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"

namespace mce
{
    class WorldConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantMatrix4x4* WORLDVIEWPROJ;
        ShaderConstantMatrix4x4* WORLD;
        ShaderConstantMatrix4x4* WORLDVIEW;
        ShaderConstantMatrix4x4* PROJ;

    public:
        WorldConstants();

        void refreshWorldConstants();

        void init();
    };
}
