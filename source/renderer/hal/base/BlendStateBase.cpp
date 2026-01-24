#include "BlendStateBase.hpp"

using namespace mce;

BlendStateBase::BlendStateBase()
{
    m_description = BlendStateDescription();
}

void BlendStateBase::createBlendState(RenderContext& context, const BlendStateDescription& desc)
{
    m_description = desc;
}

bool BlendStateBase::bindBlendState(RenderContext& context)
{
    return context.m_currentState.m_blendStateDescription == m_description;
}
