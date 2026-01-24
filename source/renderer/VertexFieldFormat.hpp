#pragma once

#include <stddef.h>

#include "renderer/hal/enums/VertexField.hpp"

namespace mce
{
    struct VertexFieldFormat
    {
        VertexFieldType type;
        size_t size;
        unsigned int components;
        bool normalized;
    };

    class VertexFieldFormats
    {
    private:
        static const VertexFieldFormat _vertexFieldFormats[VERTEX_FIELD_TYPES_COUNT];

    public:
        static inline const VertexFieldFormat& GetFormatByType(VertexFieldType vertexFieldType)
        {
            return _vertexFieldFormats[vertexFieldType];
        }
        static inline const VertexFieldFormat& GetFormatByField(VertexField vertexField)
        {
            return GetFormatByType(vertexFieldTypes[vertexField]);
        }
    };
}
