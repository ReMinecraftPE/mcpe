#pragma once

#include "renderer/hal/RenderStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class RenderStateBase
    {
    public:
        RenderStateDescription m_description;

    public:
        RenderStateBase();

    public:
        void createRenderState(RenderContext& context, const RenderStateDescription& desc);
        bool bindRenderState(RenderContext& context);
    };
}
