#pragma once

#include "renderer/hal/ogl/BufferOGL.hpp"

namespace mce
{
    class Buffer : public BufferOGL
    {
    public:
        void createIndexBuffer(RenderContext& context, unsigned int sizeOfSingleIndice, const void *indices, unsigned int numberOfIndices);
        void createVertexBuffer(RenderContext& context, unsigned int vertexStride, const void *vertices, unsigned int numberOfVertices);
        void createDynamicIndexBuffer(RenderContext& context, unsigned int size);
        void createDynamicVertexBuffer(RenderContext& context, unsigned int size);

    public:
        Buffer& operator=(Buffer&& other);
    };
}