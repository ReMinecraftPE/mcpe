#include "ImmediateBufferBase.hpp"

using namespace mce;

ImmediateBufferBase::ImmediateBufferBase()
{
    m_bufferType = BUFFER_TYPE_NONE;
}

void ImmediateBufferBase::createDynamicBuffer(RenderContext& context, unsigned int bufferSize, BufferType bufferType, const void *data)
{
    m_bufferType = bufferType;
}

void ImmediateBufferBase::updateBuffer(RenderContext& context, unsigned int itemSize, void const* data, unsigned int bufferSize)
{
}

bool ImmediateBufferBase::isValid() const
{
    return false;
}