#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/FixedPipelineStateBase.hpp"

namespace mce
{
	class FixedPipelineStateOGL : public FixedPipelineStateBase
	{
    public:
        bool m_bAlphaTest;
        GLenum m_alphaFunc;
        float m_alphaRef;
        bool m_bTexture;

    public:
        FixedPipelineStateOGL();

    public:
        void createFixedPipelineState(RenderContext& context, const FixedPipelineStateDescription& desc);
        bool bindFixedPipelineState(RenderContext& context, bool forceBind = false);
	};
}