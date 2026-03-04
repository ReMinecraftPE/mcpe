#include "AlphaStateOGL.hpp"

#ifndef FEATURE_GFX_SHADERS

using namespace mce;

AlphaStateOGL::AlphaStateOGL()
{
    m_bAlphaTest = false;
    m_alphaFunc = GL_NONE;
    m_alphaRef = 0.0f;
}

void AlphaStateOGL::createAlphaState(RenderContext& context, const AlphaStateDescription& desc)
{
    AlphaStateBase::createAlphaState(context, desc);

    m_bAlphaTest = desc.enableAlphaTest;
    m_alphaFunc = getComparisonFunc(desc.alphaFunc);
    m_alphaRef = desc.alphaRef;

    if (!context.m_currentState.m_bBoundAlphaState)
    {
        bindAlphaState(context, true);
        context.m_currentState.m_bBoundAlphaState = true;
        context.m_currentState.m_alphaStateDescription = desc;
    }
}

bool AlphaStateOGL::bindAlphaState(RenderContext& context, bool forceBind)
{
    AlphaStateDescription& ctxDesc = context.m_currentState.m_alphaStateDescription;

    if (forceBind || ctxDesc.enableAlphaTest != m_description.enableAlphaTest)
    {
        if (m_bAlphaTest) glEnable(GL_ALPHA_TEST);
        else              glDisable(GL_ALPHA_TEST);
        ctxDesc.enableAlphaTest = m_description.enableAlphaTest;
    }

    if (forceBind || ctxDesc.alphaFunc != m_description.alphaFunc)
    {
        glAlphaFunc(m_alphaFunc, m_alphaRef);
        ctxDesc.alphaFunc = m_description.alphaFunc;
        ctxDesc.alphaRef = m_description.alphaRef;
    }

    return AlphaStateBase::bindAlphaState(context);
}

#endif // !defined(FEATURE_GFX_SHADERS)
