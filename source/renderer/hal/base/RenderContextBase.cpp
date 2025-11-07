#include "RenderContextBase.hpp"

using namespace mce;

RenderContextBase::RenderContextBase()
{
    m_renderDevice = nullptr;
}

RenderDevice* RenderContextBase::getDevice()
{
    return m_renderDevice;
}

void RenderContextBase::lostContext()
{
    m_immediateBuffer = ImmediateBuffer();
    field_38 = 0;
    field_3C = 0;
    field_40 = 0;
}

void RenderContextBase::setStencilReference(unsigned int value)
{
    m_stencilReference = value;
}

unsigned int RenderContextBase::getStencilReference() const
{
    return m_stencilReference;
}