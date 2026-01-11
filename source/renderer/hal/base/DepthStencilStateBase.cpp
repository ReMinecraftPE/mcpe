#include "DepthStencilStateBase.hpp"

using namespace mce;

DepthStencilStateBase::DepthStencilStateBase()
{
}

void DepthStencilStateBase::createDepthState(RenderContext& context, const DepthStencilStateDescription& description)
{
    m_description = description;
}

bool DepthStencilStateBase::bindDepthStencilState(RenderContext& context)
{
    return context.m_currentState.m_depthStencilStateDescription == m_description;
}
