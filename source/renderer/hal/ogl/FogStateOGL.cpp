#include <typeinfo>

#include "FogStateOGL.hpp"

#ifndef FEATURE_GFX_SHADERS

using namespace mce;

FogStateOGL::FogStateOGL()
{
    m_bFog = false;
    m_fogDensity = 1.0f;
    m_fogStart = 0.0f;
    m_fogEnd = 1.0f;
    m_fogMode = GL_EXP;
}

GLenum FogStateOGL::translateFogMode(FogMode fogMode)
{
    switch (fogMode)
    {
    case FOG_MODE_LINEAR: return GL_LINEAR;
    case FOG_MODE_EXP:    return GL_EXP;
    case FOG_MODE_EXP2:   return GL_EXP2;
    default:
        LOG_E("Unknown fogMode: %d", fogMode);
        throw std::bad_cast();
    }
}

void FogStateOGL::createFogState(RenderContext& context, const FogStateDescription& desc)
{
    FogStateBase::createFogState(context, desc);

    m_bFog = desc.enableFog;
    m_fogMode = translateFogMode(desc.fogMode);
    m_fogDensity = desc.fogDensity;
    m_fogStart = desc.fogStartZ;
    m_fogEnd = desc.fogEndZ;
    m_fogColor = desc.fogColor;

    if (!context.m_currentState.m_bBoundFogState)
    {
        bindFogState(context, true);
        context.m_currentState.m_bBoundFogState = true;
        context.m_currentState.m_fogStateDescription = desc;
    }
}

bool FogStateOGL::bindFogState(RenderContext& context, bool forceBind)
{
    FogStateDescription& ctxDesc = context.m_currentState.m_fogStateDescription;

#ifndef __EMSCRIPTEN__
    glNormal3f(0.0f, -1.0f, 0.0f);
#endif

    if (forceBind || ctxDesc.enableFog != m_description.enableFog)
    {
        if (m_bFog) glEnable(GL_FOG);
        else        glDisable(GL_FOG);
        ctxDesc.enableFog = m_description.enableFog;
    }

    if (forceBind || ctxDesc.fogMode != m_description.fogMode)
    {
#if defined(ANDROID) || defined(__SWITCH__)
        glFogx(GL_FOG_MODE, m_fogMode);
#else
        glFogi(GL_FOG_MODE, m_fogMode);
#endif
        ctxDesc.fogMode = m_description.fogMode;
    }

    if (forceBind || ctxDesc.fogDensity != m_description.fogDensity)
    {
        glFogf(GL_FOG_DENSITY, m_fogDensity);
        ctxDesc.fogDensity = m_description.fogDensity;
    }

    if (forceBind || ctxDesc.fogStartZ != m_description.fogStartZ)
    {
        glFogf(GL_FOG_START, m_fogStart);
        ctxDesc.fogStartZ = m_description.fogStartZ;
    }

    if (forceBind || ctxDesc.fogEndZ != m_description.fogEndZ)
    {
        glFogf(GL_FOG_END, m_fogEnd);
        ctxDesc.fogEndZ = m_description.fogEndZ;
    }

    if (forceBind || ctxDesc.fogColor != m_description.fogColor)
    {
        glFogfv(GL_FOG_COLOR, (float*)&m_fogColor);
        ctxDesc.fogColor = m_description.fogColor;
    }

#if !defined(__EMSCRIPTEN__) && !defined(USE_GLES)
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
#endif

    return FogStateBase::bindFogState(context);
}

#endif // !defined(FEATURE_GFX_SHADERS)