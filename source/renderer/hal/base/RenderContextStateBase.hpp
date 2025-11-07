#pragma once

#include "renderer/hal/BlendStateDescription.hpp"
#include "renderer/hal/DepthStencilStateDescription.hpp"
#include "renderer/hal/RasterizerStateDescription.hpp"

namespace mce
{
    class RenderContextStateBase
    {
    public:
        BlendStateDescription m_blendStateDescription;
        DepthStencilStateDescription m_depthStencilStateDescription;
        RasterizerStateDescription m_rasterizerStateDescription;
        bool m_bBoundBlendState;
        bool m_bBoundDepthStencilState;
        bool m_bBoundRasterizerState;

    public:
        RenderContextStateBase();
    };
}