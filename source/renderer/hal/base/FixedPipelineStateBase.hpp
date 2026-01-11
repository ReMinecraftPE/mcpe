#pragma once

#include "renderer/VertexFormat.hpp"
#include "renderer/hal/FixedPipelineStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class FixedPipelineStateBase
    {
    protected:
        VertexFormat m_lastVertexFormat;
    public:
        FixedPipelineStateDescription m_description;

    public:
        FixedPipelineStateBase();

    public:
        void createFixedPipelineState(RenderContext& context, const FixedPipelineStateDescription& desc);
        bool bindFixedPipelineState(RenderContext& context);
    };
}
