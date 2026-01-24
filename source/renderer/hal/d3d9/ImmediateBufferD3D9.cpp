#include <stdexcept>

#include "ImmediateBufferD3D9.hpp"
#include "renderer/hal/interface/RenderContext.hpp"
#include "BufferD3D9.hpp"

using namespace mce;

void ImmediateBufferD3D9::_move(ImmediateBufferD3D9& other)
{
    std::swap(this->m_buffer1, other.m_buffer1);
    std::swap(this->m_buffer2, other.m_buffer2);
    // Might land us on the wrong buffer, but whatever
    m_pBuffer = &m_buffer1;
}

ImmediateBufferD3D9::ImmediateBufferD3D9()
    : ImmediateBufferBase()
{
    m_pBuffer = &m_buffer1;
}

void ImmediateBufferD3D9::_swapBuffers()
{
    m_pBuffer = m_pBuffer == &m_buffer1 ? &m_buffer2 : &m_buffer1;
}

void ImmediateBufferD3D9::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    // We're eating 2 MBs here instead of one
    m_buffer1.createDynamicBuffer(context, stride, data, count, bufferType);
    m_buffer2.createDynamicBuffer(context, stride, data, count, bufferType);
}

void ImmediateBufferD3D9::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    // @NOTE: We do our own double-buffering because the 360 will shit itself otherwise and read the wrong vertex data
    _swapBuffers();

    MapType mapType = MAP_WRITE_NO_OVERWRITE;

    if (m_pBuffer->m_bufferOffset + count * stride > m_pBuffer->m_count)
    {
        m_pBuffer->m_bufferOffset = 0;
        mapType = MAP_WRITE_DISCARD;
    }

    m_pBuffer->updateBuffer(context, stride, data, count, mapType);
    m_pBuffer->bindBuffer(context);

    m_pBuffer->m_bufferOffset += count * stride;
}

bool ImmediateBufferD3D9::isValid() const
{
    return m_pBuffer->isValid();
}