#pragma once

#include "API_OGL.hpp"

#ifndef FEATURE_GFX_SHADERS

#include "renderer/hal/base/FogStateBase.hpp"

namespace mce
{
    class FogStateOGL : public FogStateBase
    {
    public:
        bool m_bFog;
        GLenum m_fogMode;
        float m_fogDensity;
        float m_fogStart;
        float m_fogEnd;
        Color m_fogColor;

    public:
        FogStateOGL();

    public:
        GLenum translateFogMode(FogMode fogMode);

        void createFogState(RenderContext& context, const FogStateDescription& desc);
        bool bindFogState(RenderContext& context, bool forceBind = false);
    };
}

#endif // !defined(FEATURE_GFX_SHADERS)
