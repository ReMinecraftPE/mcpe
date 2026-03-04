#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/RenderStateBase.hpp"

namespace mce
{
    class RenderStateD3D9 : public RenderStateBase
    {
    public:

    public:
        RenderStateD3D9();

    public:
        void createRenderState(RenderContext& context, const RenderStateDescription& desc);
        bool bindRenderState(RenderContext& context, bool forceBind = false);
    };
}
