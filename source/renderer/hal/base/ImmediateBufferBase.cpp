#include "ImmediateBufferBase.hpp"

using namespace mce;

ImmediateBufferBase::ImmediateBufferBase()
{
}

void ImmediateBufferBase::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
}

void ImmediateBufferBase::updateBuffer(RenderContext& context, unsigned int itemSize, void const* data, unsigned int bufferSize)
{
}

bool ImmediateBufferBase::isValid() const
{
    return false;
}
