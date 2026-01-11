#include "FixedPipelineStateOGL.hpp"

#ifndef FEATURE_GFX_SHADERS

using namespace mce;

FixedPipelineStateOGL::FixedPipelineStateOGL()
{
    m_bAlphaTest = false;
    m_alphaFunc = GL_NONE;
    m_alphaRef = 0.0f;
    m_bTexture = false;
}

void FixedPipelineStateOGL::createFixedPipelineState(RenderContext& context, const FixedPipelineStateDescription& desc)
{
    FixedPipelineStateBase::createFixedPipelineState(context, desc);

    m_bAlphaTest = desc.enableAlphaTest;
    m_alphaFunc = getComparisonFunc(desc.alphaFunc);
    m_alphaRef = desc.alphaRef;
    m_bTexture = desc.enableTexture;

    if (!context.m_currentState.m_bBoundFixedPipelineState)
    {
        bindFixedPipelineState(context, true);
        context.m_currentState.m_bBoundFixedPipelineState = true;
        context.m_currentState.m_fixedPipelineStateDescription = desc;
    }
}

bool FixedPipelineStateOGL::bindFixedPipelineState(RenderContext& context, bool forceBind)
{
    FixedPipelineStateDescription& ctxDesc = context.m_currentState.m_fixedPipelineStateDescription;

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

    if (forceBind || ctxDesc.enableTexture != m_description.enableTexture)
    {
        if (m_bTexture) glEnable(GL_TEXTURE_2D);
        else            glDisable(GL_TEXTURE_2D);
        ctxDesc.enableTexture = m_description.enableTexture;
    }

    return FixedPipelineStateBase::bindFixedPipelineState(context);
}

#endif // !defined(FEATURE_GFX_SHADERS)