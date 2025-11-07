#pragma once

#include "enums/ShaderStagesBits.hpp"

namespace mce
{
    class ShaderStage
    {
    public:
        ShaderStagesBits m_shaderStageBitsEnum;
        unsigned int m_shaderStageBits;
    };
}