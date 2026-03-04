#include "RenderStateBase.hpp"

using namespace mce;

RenderStateBase::RenderStateBase()
{
}

void RenderStateBase::createRenderState(RenderContext& context, const RenderStateDescription& desc)
{
    m_description = desc;
}

bool RenderStateBase::bindRenderState(RenderContext& context)
{
    return context.m_currentState.m_renderStateDescription == m_description;
}
