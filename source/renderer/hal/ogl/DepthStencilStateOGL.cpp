#include "DepthStencilStateOGL.hpp"

using namespace mce;

DepthStencilStateOGL::DepthStencilStateOGL()
    : DepthStencilStateBase()
{
    m_depthWriteMask = DEPTH_WRITE_MASK_ALL;
    m_depthFunc = GL_NONE;
    m_stencilReadMask = 0;
    m_stencilWriteMask = 0;
}

GLenum getStencilOpAction(StencilOp stencilOp)
{
    switch (stencilOp)
    {
    case STENCIL_OP_KEEP:    return GL_KEEP;
    case STENCIL_OP_REPLACE: return GL_REPLACE;
    default:
        LOG_E("Unknown stencilOp: %d", stencilOp);
        throw std::bad_cast();
    }
}

void DepthStencilStateOGL::createDepthState(RenderContext& context, const DepthStencilStateDescription& description)
{
    *this = DepthStencilStateOGL();
    DepthStencilStateBase::createDepthState(context, description);

    m_depthFunc = getComparisonFunc(description.depthFunc);

    m_frontFaceStencilInfo.func = getComparisonFunc(description.frontFace.stencilFunc);
    m_frontFaceStencilInfo.stencilFailAction = getStencilOpAction(description.frontFace.stencilFailOp);
    m_frontFaceStencilInfo.stencilPassDepthFailAction = getStencilOpAction(description.frontFace.stencilDepthFailOp);
    m_frontFaceStencilInfo.stencilPassDepthPassAction = getStencilOpAction(description.frontFace.stencilPassOp);

    m_backFaceStencilInfo.func = getComparisonFunc(description.backFace.stencilFunc);
    m_backFaceStencilInfo.stencilFailAction = getStencilOpAction(description.backFace.stencilFailOp);
    m_backFaceStencilInfo.stencilPassDepthFailAction = getStencilOpAction(description.backFace.stencilDepthFailOp);
    m_backFaceStencilInfo.stencilPassDepthPassAction = getStencilOpAction(description.backFace.stencilPassOp);

    m_depthWriteMask = description.depthWriteMask;
    m_stencilReadMask = description.stencilReadMask;
    m_stencilWriteMask = description.stencilWriteMask;

    if (!context.m_currentState.m_bBoundDepthStencilState)
    {
        bindDepthStencilState(context, true);
        context.m_currentState.m_depthStencilStateDescription.depthTestEnabled = description.depthTestEnabled;
        context.m_currentState.m_depthStencilStateDescription.frontFace.stencilDepthFailOp = description.frontFace.stencilDepthFailOp;
        context.m_currentState.m_depthStencilStateDescription.backFace.stencilDepthFailOp = description.backFace.stencilDepthFailOp;
        context.m_currentState.m_depthStencilStateDescription.stencilReadMask = description.stencilReadMask;
        context.m_currentState.m_depthStencilStateDescription.stencilWriteMask = description.stencilWriteMask;
        context.m_currentState.m_depthStencilStateDescription.stencilRef = description.stencilRef;
        context.m_currentState.m_depthStencilStateDescription.overwroteStencilRef = description.overwroteStencilRef;
        context.m_currentState.m_bBoundDepthStencilState = true;
    }
}

bool DepthStencilStateOGL::bindDepthStencilState(RenderContext& context, bool force)
{
    if (!m_description.overwroteStencilRef)
    {
        m_description.stencilRef = context.getStencilReference();
    }

    DepthStencilStateDescription& currentDesc = context.m_currentState.m_depthStencilStateDescription;

    if (force || currentDesc.depthTestEnabled != m_description.depthTestEnabled)
    {
        if (m_description.depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        currentDesc.depthTestEnabled = m_description.depthTestEnabled;
    }

    if (force || currentDesc.stencilTestEnabled != m_description.stencilTestEnabled)
    {
        if (m_description.stencilTestEnabled)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
        
        currentDesc.stencilTestEnabled = m_description.stencilTestEnabled;
    }

    if (force
        || currentDesc.frontFace.stencilFunc != m_description.frontFace.stencilFunc
        || currentDesc.backFace.stencilFunc != m_description.backFace.stencilFunc
        || currentDesc.stencilReadMask != m_description.stencilReadMask
        || currentDesc.stencilRef != m_description.stencilRef)
    {
#ifdef USE_GL_STENCIL_SEPARATE
        xglStencilFuncSeparate(GL_FRONT, m_frontFaceStencilInfo.func, m_description.stencilRef, m_stencilReadMask);
        xglStencilFuncSeparate(GL_BACK, m_backFaceStencilInfo.func, m_description.stencilRef, m_stencilReadMask);
#else
        // This shit's running on thoughts and prayers
        glStencilFunc(m_frontFaceStencilInfo.func, m_description.stencilRef, m_stencilReadMask);
#endif

        currentDesc.frontFace.stencilFunc = m_description.frontFace.stencilFunc;
        currentDesc.backFace.stencilFunc = m_description.backFace.stencilFunc;
        currentDesc.stencilReadMask = m_description.stencilReadMask;
        currentDesc.stencilRef = m_description.stencilRef;
    }

#ifdef USE_GL_STENCIL_SEPARATE
    if (force || currentDesc.frontFace.stencilDepthFailOp != m_description.frontFace.stencilDepthFailOp)
    {
        xglStencilOpSeparate(GL_FRONT, m_frontFaceStencilInfo.stencilFailAction, m_frontFaceStencilInfo.stencilPassDepthFailAction, m_frontFaceStencilInfo.stencilPassDepthPassAction);
        currentDesc.frontFace.stencilDepthFailOp = m_description.frontFace.stencilDepthFailOp;
        currentDesc.frontFace.stencilPassOp = m_description.frontFace.stencilPassOp;
        currentDesc.frontFace.stencilFailOp = m_description.frontFace.stencilFailOp;
    }

    if (force || currentDesc.backFace.stencilDepthFailOp != m_description.backFace.stencilDepthFailOp)
    {
        xglStencilOpSeparate(GL_BACK, m_backFaceStencilInfo.stencilFailAction, m_backFaceStencilInfo.stencilPassDepthFailAction, m_backFaceStencilInfo.stencilPassDepthPassAction);
        currentDesc.backFace.stencilDepthFailOp = m_description.backFace.stencilDepthFailOp;
        currentDesc.backFace.stencilPassOp = m_description.backFace.stencilPassOp;
        currentDesc.backFace.stencilFailOp = m_description.backFace.stencilFailOp;
    }
#else
    if (force || currentDesc.frontFace.stencilDepthFailOp != m_description.frontFace.stencilDepthFailOp
              || currentDesc.backFace.stencilDepthFailOp != m_description.backFace.stencilDepthFailOp)
    {
        // and this shit's running on hopes and dreams
        glStencilOp(m_frontFaceStencilInfo.stencilFailAction, m_frontFaceStencilInfo.stencilPassDepthFailAction, m_frontFaceStencilInfo.stencilPassDepthPassAction);
        
        currentDesc.frontFace.stencilDepthFailOp = m_description.frontFace.stencilDepthFailOp;
        currentDesc.frontFace.stencilPassOp = m_description.frontFace.stencilPassOp;
        currentDesc.frontFace.stencilFailOp = m_description.frontFace.stencilFailOp;
        
        currentDesc.backFace.stencilDepthFailOp = m_description.backFace.stencilDepthFailOp;
        currentDesc.backFace.stencilPassOp = m_description.backFace.stencilPassOp;
        currentDesc.backFace.stencilFailOp = m_description.backFace.stencilFailOp;
    }
#endif
    
    if (force || currentDesc.stencilWriteMask != m_description.stencilWriteMask)
    {
        glStencilMask(m_stencilWriteMask);
        currentDesc.stencilWriteMask = m_description.stencilWriteMask;
    }

    if (force || currentDesc.depthFunc != m_description.depthFunc)
    {
        glDepthFunc(m_depthFunc);
        currentDesc.depthFunc = m_description.depthFunc;
    }

    if (force || currentDesc.depthWriteMask != m_description.depthWriteMask)
    {
        glDepthMask(m_depthWriteMask);
        currentDesc.depthWriteMask = m_description.depthWriteMask;
    }

    currentDesc.overwroteStencilRef = m_description.overwroteStencilRef;
    
    
    return DepthStencilStateBase::bindDepthStencilState(context);
}
