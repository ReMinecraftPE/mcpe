#pragma once

#include "VertexFieldType.hpp"

namespace mce
{
    // @NOTE: See VertexField.cpp for VertexField types
    enum VertexField
    {
        VERTEX_FIELD_POSITION,
        VERTEX_FIELD_COLOR,
        VERTEX_FIELD_NORMAL,
        VERTEX_FIELD_UV0,
        VERTEX_FIELD_UV1,
        VERTEX_FIELD_UV2,
        VERTEX_FIELD_PBR_IDX,
        VERTEX_FIELD_BONEID_0,
        VERTEX_FIELD_PREVIOUS_POSITION,
        VERTEX_FIELD_HB_POSITION,
        VERTEX_FIELD_HB_COLOR,
        VERTEX_FIELD_HB_ADDITIONAL,

        VERTEX_FIELDS_MIN = VERTEX_FIELD_POSITION,
        VERTEX_FIELDS_MAX = VERTEX_FIELD_BONEID_0,
        VERTEX_FIELDS_COUNT,

        VERTEX_FIELD_INVALID = 0xFFFF
    };

    extern const char* vertexFieldNames[VERTEX_FIELDS_COUNT];
    extern const VertexFieldType vertexFieldTypes[VERTEX_FIELDS_COUNT];
}
