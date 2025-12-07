#include <typeinfo>
#include "ShaderPrimitiveTypes.hpp"
#include "common/Logger.hpp"

using namespace mce;

int ShaderPrimitiveTypeHelper::sizeInBytesFromShaderPrimitiveType(ShaderPrimitiveTypes shaderPrimitiveType)
{
    switch (shaderPrimitiveType)
    {
        case SHADER_PRIMITIVE_FLOAT1:
        case SHADER_PRIMITIVE_INT1:
            return 4;
        case SHADER_PRIMITIVE_FLOAT2:
        case SHADER_PRIMITIVE_INT2:
            return 8;
        case SHADER_PRIMITIVE_FLOAT3:
        case SHADER_PRIMITIVE_INT3:
            return 12;
        case SHADER_PRIMITIVE_FLOAT4:
        case SHADER_PRIMITIVE_INT4:
        case SHADER_PRIMITIVE_MATRIX2x2:
            return 16;
        case SHADER_PRIMITIVE_MATRIX3x3:
            return 36;
        case SHADER_PRIMITIVE_MATRIX4x4:
            return 64;
        default:
            LOG_E("Unknown shaderPrimitiveType: %d", shaderPrimitiveType);
            throw std::bad_cast();
    }
}
