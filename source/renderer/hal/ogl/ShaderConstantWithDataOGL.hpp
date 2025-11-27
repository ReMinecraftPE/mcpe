#pragma once

#include "API_OGL.hpp"
#include "GameMods.hpp"

#ifdef FEATURE_GFX_SHADERS

#include "renderer/hal/base/ShaderConstantWithDataBase.hpp"

namespace mce
{
    template <ShaderPrimitiveTypes T>
    class ShaderConstantWithDataOGL : public ShaderConstantWithDataBase
    {
    public:
        ShaderConstantWithDataOGL(ShaderPrimitiveTypes primitiveType)
            : ShaderConstantWithDataBase(primitiveType)
        {
        }

        void syncUniform(int value);
    };
}

#endif // FEATURE_GFX_SHADERS