#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/RasterizerStateBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class RasterizerStateD3D9 : public RasterizerStateBase
    {
    public:
        RasterizerStateD3D9();

    public:
        void createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc);
        bool bindRasterizerState(RenderContext& context, bool forceBind = false);
        void setScissorRect(RenderContext &context, int x, int y, int width, int height);
    };
}