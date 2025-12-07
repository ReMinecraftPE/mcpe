#pragma once

namespace mce
{
    enum ShaderStagesBits
    {
        SHADER_STAGE_BIT_NO_SHADER = 0x0,
        SHADER_STAGE_BIT_VERTEX = (1 << 0),
        SHADER_STAGE_BIT_PIXEL = (1 << 1),
        SHADER_STAGE_BIT_GEOMETRY = (1 << 2),

        SHADER_STAGE_BITS_ALL = SHADER_STAGE_BIT_VERTEX | SHADER_STAGE_BIT_PIXEL | SHADER_STAGE_BIT_GEOMETRY
    };
}