#include <typeinfo>

#include "BlendStateOGL.hpp"

using namespace mce;

BlendStateOGL::BlendStateOGL()
{
    m_bBlend = false;
    m_bRed = true;
    m_bGreen = true;
    m_bBlue = true;
    m_bAlpha = true;
    m_sfactor = GL_NONE;
    m_dfactor = GL_NONE;
}

GLenum BlendStateOGL::translateBlendFunc(BlendTarget blendTarget)
{
    switch (blendTarget)
    {
    case BLEND_TARGET_DEST_COLOR:           return GL_DST_COLOR;
    case BLEND_TARGET_SOURCE_COLOR:         return GL_SRC_COLOR;
    case BLEND_TARGET_ZERO:                 return GL_ZERO;
    case BLEND_TARGET_ONE:                  return GL_ONE;
    case BLEND_TARGET_ONE_MINUS_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
    case BLEND_TARGET_ONE_MINUS_SRC_COLOR:  return GL_ONE_MINUS_SRC_COLOR;
    case BLEND_TARGET_SOURCE_ALPHA:         return GL_SRC_ALPHA;
    case BLEND_TARGET_ONE_MINUS_SRC_ALPHA:  return GL_ONE_MINUS_SRC_ALPHA;
    default:
        LOG_E("Unknown blendFunc: %d", blendTarget);
        throw std::bad_cast();
    }
}

void BlendStateOGL::createBlendState(RenderContext& context, const BlendStateDescription& desc)
{
    BlendStateBase::createBlendState(context, desc);
    m_bBlend  = desc.enableBlend;
    m_bRed    = (desc.colorWriteMask & 1) != 0;
    m_bGreen  = (desc.colorWriteMask & 2) != 0;
    m_bBlue   = (desc.colorWriteMask & 4) != 0;
    m_bAlpha  = (desc.colorWriteMask & 8) != 0;
    m_sfactor = translateBlendFunc(desc.blendSource);
    m_dfactor = translateBlendFunc(desc.blendDestination);

    if (!context.m_currentState.m_bBoundBlendState)
    {
        bindBlendState(context, true);
        context.m_currentState.m_bBoundBlendState = true;
        context.m_currentState.m_blendStateDescription = desc;
    }
}

bool BlendStateOGL::bindBlendState(RenderContext& context, bool forceBind)
{
    BlendStateDescription& ctxDesc = context.m_currentState.m_blendStateDescription;

    if (forceBind || ctxDesc.enableBlend != m_description.enableBlend)
    {
        if (m_bBlend) glEnable(GL_BLEND);
        else          glDisable(GL_BLEND);
        ctxDesc.enableBlend = m_description.enableBlend;
    }

    if (forceBind || ctxDesc.colorWriteMask != m_description.colorWriteMask)
    {
        glColorMask(m_bRed, m_bGreen, m_bBlue, m_bAlpha);
        ctxDesc.colorWriteMask = m_description.colorWriteMask;
    }

    if (forceBind || ctxDesc.blendSource != m_description.blendSource || ctxDesc.blendDestination != m_description.blendDestination)
    {
        glBlendFunc(m_sfactor, m_dfactor);
        ctxDesc.blendSource = m_description.blendSource;
        ctxDesc.blendDestination = m_description.blendDestination;
    }

    return BlendStateBase::bindBlendState(context); 
}