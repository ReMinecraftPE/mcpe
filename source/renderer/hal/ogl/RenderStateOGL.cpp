#include "RenderStateOGL.hpp"

#ifndef FEATURE_GFX_SHADERS

using namespace mce;

RenderStateOGL::RenderStateOGL()
{
    m_bTexture = false;
}

void RenderStateOGL::createRenderState(RenderContext& context, const RenderStateDescription& desc)
{
    RenderStateBase::createRenderState(context, desc);

    m_bTexture = desc.enableTexture;

    if (!context.m_currentState.m_bBoundRenderState)
    {
        bindRenderState(context, true);
        context.m_currentState.m_bBoundRenderState = true;
        context.m_currentState.m_renderStateDescription = desc;
    }
}

bool RenderStateOGL::bindRenderState(RenderContext& context, bool forceBind)
{
    RenderStateDescription& ctxDesc = context.m_currentState.m_renderStateDescription;

    if (forceBind || ctxDesc.enableTexture != m_description.enableTexture)
    {
        if (m_bTexture) glEnable(GL_TEXTURE_2D);
        else            glDisable(GL_TEXTURE_2D);
        ctxDesc.enableTexture = m_description.enableTexture;
    }

    return RenderStateBase::bindRenderState(context);
}

#endif // !defined(FEATURE_GFX_SHADERS)
