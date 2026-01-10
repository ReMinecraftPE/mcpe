#include "FogStateBase.hpp"

using namespace mce;

FogStateBase::FogStateBase()
{
    m_description = FogStateDescription();
}

void FogStateBase::createFogState(RenderContext& context, const FogStateDescription& desc)
{
    m_description = desc;
}

bool FogStateBase::bindFogState(RenderContext& context)
{
    return context.m_currentState.m_fogStateDescription == m_description;
}
