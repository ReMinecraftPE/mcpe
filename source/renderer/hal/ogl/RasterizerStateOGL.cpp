#include <typeinfo>
#include "RasterizerStateOGL.hpp"

using namespace mce;

RasterizerStateOGL::RasterizerStateOGL()
    : RasterizerStateBase()
{
    m_cullMode = true;
    m_depthBias = 0.0f;
    m_cullFace = GL_NONE;
    m_enableScissorTest = false;
}

bool RasterizerStateOGL::bindRasterizerState(RenderContext& context, bool forceBind)
{
    RasterizerStateDescription& ctxDesc = context.m_currentState.m_rasterizerStateDescription;

    if (forceBind || ctxDesc.cullMode != m_description.cullMode)
    {
        if (m_cullMode)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_cullFace);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        ctxDesc.cullMode = m_description.cullMode;
    }

    if (forceBind || ctxDesc.enableScissorTest != m_description.enableScissorTest)
    {
        if (m_enableScissorTest)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
        ctxDesc.enableScissorTest = m_description.enableScissorTest;
    }

    if (forceBind || ctxDesc.depthBias != m_description.depthBias)
    {
        if (m_depthBias == 0.0f)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        else
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
        }
        glPolygonOffset(m_depthBias * 5.0f, m_depthBias * 5.0f);
        ctxDesc.depthBias = m_description.depthBias;
    }

    return RasterizerStateBase::bindRasterizerState(context);
}

void RasterizerStateOGL::createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc)
{
    RasterizerStateBase::createRasterizerStateDescription(context, desc);
    m_enableScissorTest = desc.enableScissorTest;
    m_cullMode = desc.cullMode != CULL_NONE ? true : false;
    switch (desc.cullMode)
    {
        case CULL_NONE:
            break;
        case CULL_FRONT:
            m_cullFace = GL_FRONT;
            break;
        case CULL_BACK:
            m_cullFace = GL_BACK;
            break;
        default:
            LOG_E("Unknown cullMode: %d", desc.cullMode);
            throw std::bad_cast();
    }

    m_depthBias = desc.depthBias;
    if ( !context.m_currentState.m_bBoundRasterizerState )
    {
        bindRasterizerState(context, true);
        context.m_currentState.m_rasterizerStateDescription = desc;
        context.m_currentState.m_bBoundRasterizerState = true;
    }
}

void RasterizerStateOGL::setScissorRect(RenderContext& context, int x, int y, int width, int height)
{
    if (m_enableScissorTest)
        glScissor(x, y, width, height);
}
