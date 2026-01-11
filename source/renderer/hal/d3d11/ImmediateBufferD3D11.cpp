#include <stdexcept>

#include "ImmediateBufferD3D11.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "BufferD3D11.hpp"

using namespace mce;

ImmediateBufferD3D11::ImmediateBufferD3D11()
    : ImmediateBufferBase()
{
}

void ImmediateBufferD3D11::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    BufferD3D11::createDynamicBuffer(context, stride, data, count, bufferType);
}

void ImmediateBufferD3D11::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    MapType mapType = MAP_WRITE_NO_OVERWRITE;

    if (m_offset + count * stride > m_count)
    {
        m_offset = 0;
        mapType = MAP_WRITE_DISCARD;
    }

    BufferD3D11::updateBuffer(context, stride, data, count, mapType);
    BufferD3D11::bindBuffer(context);

    m_offset += count * stride;
}

bool ImmediateBufferD3D11::isValid() const
{
    return BufferD3D11::isValid();
}