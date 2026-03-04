#pragma once

#include "API_OGL.hpp"

#ifndef FEATURE_GFX_SHADERS

#include "renderer/hal/base/AlphaStateBase.hpp"

namespace mce
{
    class AlphaStateOGL : public AlphaStateBase
    {
    public:
        bool m_bAlphaTest;
        GLenum m_alphaFunc;
        float m_alphaRef;

    public:
        AlphaStateOGL();

    public:
        void createAlphaState(RenderContext& context, const AlphaStateDescription& desc);
        bool bindAlphaState(RenderContext& context, bool forceBind = false);
    };
}

#endif //!defined(FEATURE_GFX_SHADERS)
