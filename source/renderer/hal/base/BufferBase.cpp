#include <utility>
#include "BufferBase.hpp"

using namespace mce;

BufferBase::BufferBase()
{
    m_internalSize = 0;
    m_bufferOffset = 0;
    
    releaseBuffer();
}

BufferBase::~BufferBase()
{
    releaseBuffer();
}

BufferBase::BufferBase(const BufferBase& other)
{
    m_bufferType = other.m_bufferType;
    m_stride = other.m_stride;
    m_count = other.m_count;
    m_internalSize = other.m_internalSize;
}

BufferBase::BufferBase(BufferBase&& other)
{
    *this = std::move(other);
    releaseBuffer();
}

void BufferBase::releaseBuffer()
{
    m_stride = 0;
    m_bufferType = BUFFER_TYPE_NONE;
    m_count = 0;
}

void BufferBase::createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType)
{
    m_stride = stride;
    m_count = count;
    m_internalSize = count * stride;
    m_bufferType = bufferType;
}

void BufferBase::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    createBuffer(context, stride, data, count, bufferType);
}

void BufferBase::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    m_stride = stride;
    m_count = count;
}

BufferBase& BufferBase::operator=(BufferBase&& other)
{
    unsigned int stride = m_stride;
    this->m_stride = other.m_stride;
    other.m_stride = stride;

    BufferType bufferType = m_bufferType;
    this->m_bufferType = other.m_bufferType;
    other.m_bufferType = bufferType;
    
    unsigned int internalSize = m_internalSize;
    this->m_internalSize = other.m_internalSize;
    other.m_internalSize = internalSize;

    unsigned int count = m_count;
    this->m_count = other.m_count;
    other.m_count = count;

    return *this;
}