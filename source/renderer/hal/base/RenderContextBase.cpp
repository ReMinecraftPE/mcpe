#include "RenderContextBase.hpp"

using namespace mce;

RenderContextBase::RenderContextBase()
{
    m_renderDevice = nullptr;
}

void RenderContextBase::setDrawState(const VertexFormat& vertexFormat)
{
}

void RenderContextBase::clearDrawState(const VertexFormat& vertexFormat)
{
}

void RenderContextBase::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
}

void RenderContextBase::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
}

void RenderContextBase::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
}

void RenderContextBase::clearContextState()
{
    m_immediateBuffer = ImmediateBuffer();
    field_38 = 0;
    field_3C = 0;
    field_40 = 0;
}

void RenderContextBase::lostContext()
{
    clearContextState();
}

RenderDevice* RenderContextBase::getDevice()
{
    return m_renderDevice;
}

void RenderContextBase::setStencilReference(unsigned int value)
{
    m_stencilReference = value;
}

unsigned int RenderContextBase::getStencilReference() const
{
    return m_stencilReference;
}

int RenderContextBase::getMaxVertexCount()
{
    return 0;
}