#include "RasterizerStateBase.hpp"

using namespace mce;

RasterizerStateBase::RasterizerStateBase()
{
}

void RasterizerStateBase::createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc)
{
    m_description = desc;
}

bool RasterizerStateBase::bindRasterizerState(RenderContext& context)
{
    return context.m_currentState.m_rasterizerStateDescription == m_description;
}