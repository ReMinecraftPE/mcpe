#include <stdint.h>

#include "VertexFieldFormat.hpp"

using namespace mce;

#define VFF(fieldType, baseSize, components, normalized) { VERTEX_FIELD_TYPE_##fieldType, baseSize * components, components, normalized }

const VertexFieldFormat VertexFieldFormats::_vertexFieldFormats[] = {
    VFF(UNKNOWN,    0,                1, false),
    //VFF(SINT8,    sizeof(int8_t),   1, false),
    //VFF(UINT8,    sizeof(uint8_t),  1, false),
    VFF(UINT8_4,    sizeof(uint8_t),  4, false),
    VFF(UINT8_4_N,  sizeof(uint8_t),  4, true ),
    //VFF(SINT16,   sizeof(int16_t),  1, false),
    VFF(SINT16_2,   sizeof(int16_t),  2, false),
    VFF(SINT16_2_N, sizeof(int16_t),  2, true ),
    VFF(SINT16_4,   sizeof(int16_t),  4, false),
    VFF(SINT16_4_N, sizeof(int16_t),  4, true ),
    //VFF(UINT16,   sizeof(uint16_t), 1, false),
    //VFF(UINT16_2, sizeof(uint16_t), 2, false),
    VFF(UINT16_2_N, sizeof(uint16_t), 2, true ),
    VFF(UINT16_4_N, sizeof(uint16_t), 4, true ),
    //VFF(SINT32,   sizeof(int32_t),  1, false),
    //VFF(UINT32,   sizeof(uint32_t), 1, false),
    VFF(FLOAT32,    sizeof(float),    1, false),
    VFF(FLOAT32_2,  sizeof(float),    2, false),
    VFF(FLOAT32_3,  sizeof(float),    3, false),
    VFF(FLOAT32_4,  sizeof(float),    4, false)
};

#undef VFF
