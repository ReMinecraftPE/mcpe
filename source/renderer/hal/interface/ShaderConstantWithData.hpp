#pragma once

#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER_SHADER(ShaderConstantWithData)

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithData : public MCE_GFX_CLASS_SHADER(ShaderConstantWithData)<T>
    {
    public:
        ShaderConstantWithData(): MCE_GFX_CLASS_SHADER(ShaderConstantWithData)<T>(T)
        {
        }

#if MCE_GFX_API_OGL
        virtual void syncUniform(int location)
        {
            MCE_GFX_CLASS_SHADER(ShaderConstantWithData)<T>::syncUniform(location);
            ShaderConstant::syncUniform(location);
        }
#endif
    };

    class ShaderConstantFloat1    : public ShaderConstantWithData<SHADER_PRIMITIVE_FLOAT1>    {};
    class ShaderConstantFloat2    : public ShaderConstantWithData<SHADER_PRIMITIVE_FLOAT2>    {};
    class ShaderConstantFloat3    : public ShaderConstantWithData<SHADER_PRIMITIVE_FLOAT3>    {};
    class ShaderConstantFloat4    : public ShaderConstantWithData<SHADER_PRIMITIVE_FLOAT4>    {};
    class ShaderConstantInt1      : public ShaderConstantWithData<SHADER_PRIMITIVE_INT1>      {};
    class ShaderConstantInt2      : public ShaderConstantWithData<SHADER_PRIMITIVE_INT2>      {};
    class ShaderConstantInt3      : public ShaderConstantWithData<SHADER_PRIMITIVE_INT3>      {};
    class ShaderConstantInt4      : public ShaderConstantWithData<SHADER_PRIMITIVE_INT4>      {};
    class ShaderConstantMatrix2x2 : public ShaderConstantWithData<SHADER_PRIMITIVE_MATRIX2x2> {};
    class ShaderConstantMatrix3x3 : public ShaderConstantWithData<SHADER_PRIMITIVE_MATRIX3x3> {};
    class ShaderConstantMatrix4x4 : public ShaderConstantWithData<SHADER_PRIMITIVE_MATRIX4x4> {};
}
