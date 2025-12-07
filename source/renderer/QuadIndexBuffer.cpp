#include <stdint.h>
#include <vector>
#include <typeinfo>
#include "QuadIndexBuffer.hpp"

using namespace mce;

QuadIndexBuffer* QuadIndexBuffer::instance = nullptr;

QuadIndexBuffer::QuadIndexBuffer()
{
    m_capacity = 0;
    m_indexSize = 0;
}

void QuadIndexBuffer::onAppTerminated()
{
    onAppSuspended();
}

void QuadIndexBuffer::onAppSuspended()
{
    m_capacity = 1;
    m_globalBuffer.releaseBuffer();
    m_indexSize = 0;
}

template <typename T>
void _makeIndexBuffer(std::vector<uint8_t>& indices, unsigned int vertexCount)
{
    constexpr unsigned int indexSize = sizeof(T) * 6;
    const unsigned int quadCount = vertexCount / 4;
    indices.resize(indexSize * quadCount);
    for (unsigned int i = 0; i < quadCount; i++)
    {
        T baseVertex = static_cast<T>(i * 4);
        size_t baseIndex = i * indexSize;

        T* data = (T*)&indices[baseIndex];

        // The index pattern creates two triangles for each quad (v, v+1, v+2, v+3):
        // Triangle 1: (v+1, v+2, v)
        data[0] = baseVertex + 1;
        data[1] = baseVertex + 2;
        data[2] = baseVertex + 0;
        // Triangle 2: (v,   v+2, v+3)
        data[3] = baseVertex + 0;
        data[4] = baseVertex + 2;
        data[5] = baseVertex + 3;
    }
}

Buffer& QuadIndexBuffer::getGlobalQuadBuffer(RenderContext& context, unsigned int requiredCapacity, uint8_t& outIndexSize)
{
    if (m_capacity >= requiredCapacity)
    {
        outIndexSize = m_indexSize;
        return m_globalBuffer;
    }

    if (requiredCapacity == 0)
    {
        LOG_E("requiredCapacity cannot be 0");
        throw std::bad_cast();
    }

    while (m_capacity < requiredCapacity)
        m_capacity *= 2;

    std::vector<uint8_t> indices;

    // Use 16-bit indices for smaller buffers to save memory, otherwise use 32-bit.
    if (m_capacity < 0x10000 || !RenderContext::supports32BitIndices())
    {
        m_indexSize = sizeof(uint16_t);
        _makeIndexBuffer<uint16_t>(indices, m_capacity);
    }
    else
    {
        m_indexSize = sizeof(uint32_t);
        _makeIndexBuffer<uint32_t>(indices, m_capacity);
    }

    void* data = &indices[0];

    m_globalBuffer.createDynamicIndexBuffer(context, indices.size());
    m_globalBuffer.updateBuffer(context, m_indexSize, data, indices.size() / m_indexSize);

    outIndexSize = m_indexSize;
    return m_globalBuffer;
}

Buffer& QuadIndexBuffer::get(RenderContext& context, unsigned int requiredCapacity, uint8_t& outIndexSize)
{
    if (!instance)
    {
        instance = new QuadIndexBuffer();
        instance->m_capacity = 1;
    }

    return instance->getGlobalQuadBuffer(context, requiredCapacity, outIndexSize);
}

void QuadIndexBuffer::release()
{
    delete instance;
    instance = nullptr;
}