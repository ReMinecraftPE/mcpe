#include <string.h>

#include "VertexFormat.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"

using namespace mce;

const VertexFormat VertexFormat::EMPTY = VertexFormat();
const unsigned int VertexFormat::FieldSize[] = {
    /* VERTEX_FIELD_POSITION */ sizeof(Vec3),
    /* VERTEX_FIELD_COLOR */    sizeof(uint32_t),
    /* VERTEX_FIELD_NORMAL */   sizeof(uint32_t),
    /* VERTEX_FIELD_UV0 */      sizeof(Vec2), // 4 on 0.12.1. in our case, it's 8; 2 floats, one for U, and one for V
    /* VERTEX_FIELD_UV1 */      sizeof(Vec2),
    /* VERTEX_FIELD_UV2 */      0,
    /* VERTEX_FIELD_PBR_IDX */  0,
    /* VERTEX_FIELD_BONEID_0 */ 0
};

void VertexFormat::_init()
{
    m_fieldMask = 0;
    m_vertexSize = 0;
    memset(m_fieldOffset, -1, sizeof(m_fieldOffset));
}

VertexFormat::VertexFormat()
{
    _init();
}

void VertexFormat::enableField(VertexField vertexField)
{
    if (hasField(vertexField)) return;
    //if (vertexField != VERTEX_FIELD_POSITION) return;

    m_fieldOffset[vertexField] = m_vertexSize;
    uint8_t v6 = m_vertexSize + VertexFormat::FieldSize[vertexField];
    uint8_t v8 = v6 & 3;
    m_vertexSize = v6;
    if (v8)
        m_vertexSize += 4 - v8;
    m_fieldMask |= (1 << vertexField);
}

bool VertexFormat::hasField(VertexField vertexField) const
{
    return HasField(m_fieldMask, vertexField);
}

const void* VertexFormat::getFieldOffset(VertexField vertexField, const void *vertexData) const
{
    return (uint8_t*)vertexData + m_fieldOffset[vertexField];
}

bool VertexFormat::operator==(const VertexFormat &other) const
{
    return m_fieldMask == other.m_fieldMask
        && m_vertexSize == other.m_vertexSize
        && memcmp(m_fieldOffset, other.m_fieldOffset, sizeof(m_fieldOffset)) == 0;
}

bool VertexFormat::operator!=(const VertexFormat &other) const
{
    return !(*this == other);
}

bool VertexFormat::operator<(const VertexFormat &other) const
{
    return (unsigned int)memcmp(this, &other, sizeof(VertexFormat)) >> 31;
}

bool VertexFormat::HasField(uint8_t fieldMask, VertexField vertexField)
{
    return (fieldMask >> vertexField) & 1;
}