#include "FixedPipelineStateBase.hpp"

using namespace mce;

FixedPipelineStateBase::FixedPipelineStateBase()
{
}

void FixedPipelineStateBase::createFixedPipelineState(RenderContext& context, const FixedPipelineStateDescription& desc)
{
    m_description = desc;
}

bool FixedPipelineStateBase::bindFixedPipelineState(RenderContext& context)
{
    return context.m_currentState.m_fixedPipelineStateDescription == m_description;
}