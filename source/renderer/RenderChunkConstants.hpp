#pragma once

#include "renderer/hal/interface/ConstantBufferConstants.hpp"
#include "renderer/hal/interface/ShaderConstantWithData.hpp"

namespace mce
{
    class RenderChunkConstants : public ConstantBufferConstants
    {
    public:
        ShaderConstantFloat3* CHUNK_ORIGIN;

    public:
        RenderChunkConstants();

        void init();
    };
}
