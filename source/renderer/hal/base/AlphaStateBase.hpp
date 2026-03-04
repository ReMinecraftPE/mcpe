#pragma once

#include "renderer/hal/AlphaStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class AlphaStateBase
    {
    public:
        AlphaStateDescription m_description;

    public:
        AlphaStateBase();

    public:
        void createAlphaState(RenderContext& context, const AlphaStateDescription& desc);
        bool bindAlphaState(RenderContext& context);
    };
}
