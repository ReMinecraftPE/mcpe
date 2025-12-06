#include <utility>

#include "Buffer.hpp"

using namespace mce;

void Buffer::createIndexBuffer(RenderContext& context, unsigned int sizeOfSingleIndice, const void *indices, unsigned int numberOfIndices)
{
    createBuffer(context, sizeOfSingleIndice, indices, numberOfIndices, BUFFER_TYPE_INDEX);
}

void Buffer::createVertexBuffer(RenderContext& context, unsigned int vertexStride, const void *vertices, unsigned int numberOfVertices)
{
    createBuffer(context, vertexStride, vertices, numberOfVertices, BUFFER_TYPE_VERTEX);
}

void Buffer::createDynamicIndexBuffer(RenderContext& context, unsigned int stride)
{
    createDynamicBuffer(context, 1, nullptr, stride, BUFFER_TYPE_INDEX);
}

void Buffer::createDynamicVertexBuffer(RenderContext& context, unsigned int stride)
{
    createDynamicBuffer(context, 1, nullptr, stride, BUFFER_TYPE_VERTEX);
}

Buffer& Buffer::operator=(Buffer& other)
{
    this->MCE_GFX_CLASS(Buffer)::operator=(other);
    return *this;
}