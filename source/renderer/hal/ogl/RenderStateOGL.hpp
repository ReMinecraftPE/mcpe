#pragma once

#include "API_OGL.hpp"

#ifndef FEATURE_GFX_SHADERS

#include "renderer/hal/base/RenderStateBase.hpp"

namespace mce
{
	class RenderStateOGL : public RenderStateBase
	{
    public:
        bool m_bTexture;

    public:
        RenderStateOGL();

    public:
        void createRenderState(RenderContext& context, const RenderStateDescription& desc);
        bool bindRenderState(RenderContext& context, bool forceBind = false);
	};
}

#endif //!defined(FEATURE_GFX_SHADERS)
