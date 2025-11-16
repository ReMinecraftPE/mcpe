#pragma once

namespace mce
{
    enum ShaderType
    {
        SHADER_TYPE_VERTEX,
        SHADER_TYPE_FRAGMENT,
        SHADER_TYPE_GEOMETRY,
        
        SHADER_TYPES_MIN = SHADER_TYPE_VERTEX,
        SHADER_TYPES_MAX = SHADER_TYPE_GEOMETRY,
        SHADER_TYPES_COUNT
    };
}