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
    m_lastShaderPrograms[SHADER_TYPE_VERTEX] = nullptr;
    m_lastShaderPrograms[SHADER_TYPE_FRAGMENT] = nullptr;
    m_lastShaderPrograms[SHADER_TYPE_GEOMETRY] = nullptr;
}

void RenderContextBase::lostContext()
{
    clearContextState();
}

RenderDevice* RenderContextBase::getDevice()
{
    return m_renderDevice;
}

void RenderContextBase::setStencilReference(uint8_t value)
{
    m_stencilReference = value;
}

uint8_t RenderContextBase::getStencilReference() const
{
    return m_stencilReference;
}

int RenderContextBase::getMaxVertexCount()
{
    return 0;
}