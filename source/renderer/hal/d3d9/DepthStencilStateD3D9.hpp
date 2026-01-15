#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/DepthStencilStateBase.hpp"

namespace mce
{
    class DepthStencilStateD3D9 : public DepthStencilStateBase
    {
    public:
        DepthStencilStateD3D9();

    public:
        void createDepthState(RenderContext& context, const DepthStencilStateDescription& description);
        bool bindDepthStencilState(RenderContext& context, bool force = false);
    };
}