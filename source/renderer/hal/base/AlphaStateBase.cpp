#include "AlphaStateBase.hpp"

using namespace mce;

AlphaStateBase::AlphaStateBase()
{
}

void AlphaStateBase::createAlphaState(RenderContext& context, const AlphaStateDescription& desc)
{
    m_description = desc;
}

bool AlphaStateBase::bindAlphaState(RenderContext& context)
{
    return context.m_currentState.m_alphaStateDescription == m_description;
}
