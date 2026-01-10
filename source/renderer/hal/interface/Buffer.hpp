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
        void createDynamicIndexBuffer(RenderContext& context, unsigned int size);
        void createDynamicVertexBuffer(RenderContext& context, unsigned int size);

    public:
        MC_FUNC_MOVE(Buffer);
    };
}
