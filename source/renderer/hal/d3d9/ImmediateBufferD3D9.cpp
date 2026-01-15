#include <stdexcept>

#include "ImmediateBufferD3D9.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "BufferD3D9.hpp"

using namespace mce;

ImmediateBufferD3D9::ImmediateBufferD3D9()
    : ImmediateBufferBase()
{
}

void ImmediateBufferD3D9::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    BufferD3D9::createDynamicBuffer(context, stride, data, count, bufferType);
}

void ImmediateBufferD3D9::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    MapType mapType = MAP_WRITE_NO_OVERWRITE;

    if (m_offset + count * stride > m_count)
    {
        m_offset = 0;
        mapType = MAP_WRITE_DISCARD;
    }

    BufferD3D9::updateBuffer(context, stride, data, count, mapType);
    BufferD3D9::bindBuffer(context);

    m_offset += count * stride;
}

bool ImmediateBufferD3D9::isValid() const
{
    return BufferD3D9::isValid();
}