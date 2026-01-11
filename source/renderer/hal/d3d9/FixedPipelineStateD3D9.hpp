#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/FixedPipelineStateBase.hpp"

namespace mce
{
    class FixedPipelineStateD3D9 : public FixedPipelineStateBase
    {
    public:
        bool m_bAlphaTest;
        D3DCMPFUNC m_alphaFunc;
        uint8_t m_alphaRef;

    public:
        FixedPipelineStateD3D9();

    public:
        void createFixedPipelineState(RenderContext& context, const FixedPipelineStateDescription& desc);
        bool bindFixedPipelineState(RenderContext& context, bool forceBind = false);
    };
}
