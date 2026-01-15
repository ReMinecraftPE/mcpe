#pragma once

#include "compat/Unused.hpp"

#ifdef XENON
#ifdef SHADER_TYPE_VERTEX
#undef SHADER_TYPE_VERTEX
#endif
#endif

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

	MC_UNUSED static const char* ShaderTypeToString[] = {
		"SHADER_TYPE_VERTEX",
		"SHADER_TYPE_FRAGMENT",
		"SHADER_TYPE_GEOMETRY",
		"SHADER_TYPES_COUNT"
	};
}
