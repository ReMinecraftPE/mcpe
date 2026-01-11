#pragma once

#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER(Buffer)

namespace mce
{
    class Buffer : public MCE_GFX_CLASS(Buffer)
    {
    public:
        void createIndexBuffer(RenderContext& context, unsigned int sizeOfSingleIndice, const void *indices, unsigned int numberOfIndices);
        void createVertexBuffer(RenderContext& context, unsigned int vertexStride, const void *vertices, unsigned int numberOfVertices);
        void createDynamicIndexBuffer(RenderContext& context, unsigned int stride, unsigned int count);
        void createDynamicVertexBuffer(RenderContext& context, unsigned int stride, unsigned int count);

    public:
        MC_FUNC_MOVE(Buffer);
    };
}