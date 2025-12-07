#pragma once

#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "renderer/hal/interface/Buffer.hpp"

#include "VertexFormat.hpp"
#include "MaterialPtr.hpp"

namespace mce
{
    class Mesh
    {
    public:
        unsigned int m_indexCount;
        unsigned int m_vertexCount;
        PrimitiveMode m_primitiveMode;
        VertexFormat m_vertexFormat;
        uint8_t m_indexSize;
        Buffer m_vertexBuffer;
        Buffer m_indexBuffer;
        void *m_rawData;

    public:
        Mesh();
        MC_CTOR_MOVE(Mesh);
        Mesh(const VertexFormat& vertexFormat, unsigned int vertexCount, unsigned int indexCount, uint8_t indexSize, PrimitiveMode primitiveMode, uint8_t *data, bool temporary);
        ~Mesh();

    protected:
        void _move(Mesh& other);

    public:
        void reset();
        bool loadRawData(RenderContext& context, uint8_t *data);
        void render(const MaterialPtr& materialPtr, unsigned int startOffset = 0, unsigned int count = 0);
        bool isValid() const;
        bool isTemporary() const;

    public:
        MC_FUNC_MOVE(Mesh);

    public:
        static void clearGlobalBuffers();
    };
}