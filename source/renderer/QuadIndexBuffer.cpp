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
void _makeIndexBuffer(std::vector<T>& indices, unsigned int numQuads)
{
        indices.resize(numQuads * 6);
        for (unsigned int i = 0; i < numQuads; i++)
        {
            T baseVertex = static_cast<T>(i * 4);
            size_t baseIndex = i * 6;

            // The index pattern creates two triangles for each quad (v, v+1, v+2, v+3):
            // Triangle 1: (v+1, v+2, v)
            // Triangle 2: (v,   v+2, v+3)
            indices[baseIndex + 0] = baseVertex + 1;
            indices[baseIndex + 1] = baseVertex + 2;
            indices[baseIndex + 2] = baseVertex + 0;
            
            indices[baseIndex + 3] = baseVertex + 0;
            indices[baseIndex + 4] = baseVertex + 2;
            indices[baseIndex + 5] = baseVertex + 3;
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

    const unsigned int numQuads = m_capacity / 4;

    void* data;
    unsigned int dataSize;

    // Use 16-bit indices for smaller buffers to save memory, otherwise use 32-bit.
    if (m_capacity < 0x10000)
    {
        m_indexSize = sizeof(uint16_t);
        
        std::vector<uint16_t> indices;
        _makeIndexBuffer(indices, numQuads);

        data = indices.data();
        dataSize = indices.size();
    }
    else
    {
        m_indexSize = sizeof(uint32_t);

        std::vector<uint32_t> indices;
        _makeIndexBuffer(indices, numQuads);

        data = indices.data();
        dataSize = indices.size();
    }

    m_globalBuffer.createDynamicIndexBuffer(context, dataSize * m_indexSize);
    m_globalBuffer.updateBuffer(context, m_indexSize, data, dataSize);

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