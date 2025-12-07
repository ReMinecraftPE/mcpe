#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/BlendStateBase.hpp"

namespace mce
{
    class BlendStateOGL : public BlendStateBase
    {
    public:
        bool m_bBlend;
        bool m_bRed;
        bool m_bGreen;
        bool m_bBlue;
        bool m_bAlpha;
        GLenum m_sfactor;
        GLenum m_dfactor;

    public:
        BlendStateOGL();

    public:
        GLenum translateBlendFunc(BlendTarget blendTarget);

		void createBlendState(RenderContext& context, const BlendStateDescription& desc);
        bool bindBlendState(RenderContext& context, bool forceBind = false);
    };
}