#include <string.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "VertexFormat.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"
#include "GameMods.hpp"

using namespace mce;

const VertexFormat VertexFormat::EMPTY = VertexFormat();
const unsigned int VertexFormat::FieldSize[] = {
    /* VERTEX_FIELD_POSITION */ sizeof(Vec3),
    /* VERTEX_FIELD_COLOR */    sizeof(uint32_t),
    /* VERTEX_FIELD_NORMAL */   sizeof(uint32_t),
#ifdef ENH_GFX_COMPACT_UVS
    /* VERTEX_FIELD_UV0 */      sizeof(uint16_t[2]),
    /* VERTEX_FIELD_UV1 */      sizeof(uint16_t[2]),
#else
    /* VERTEX_FIELD_UV0 */      sizeof(Vec2),
    /* VERTEX_FIELD_UV1 */      sizeof(Vec2),
#endif
    /* VERTEX_FIELD_UV2 */      0,
    /* VERTEX_FIELD_PBR_IDX */  0,
    /* VERTEX_FIELD_BONEID_0 */ 0
};

void VertexFormat::_init()
{
    m_fieldMask = 0;
    m_vertexSize = 0;
    memset(m_fieldOffset, UINT8_MAX, sizeof(m_fieldOffset));
}

VertexFormat::VertexFormat()
{
    _init();
}

void VertexFormat::enableField(VertexField vertexField)
{
    if (hasField(vertexField)) return;

    m_fieldOffset[vertexField] = m_vertexSize;
    m_vertexSize = m_vertexSize + VertexFormat::FieldSize[vertexField];
    if (m_vertexSize != 4 * (m_vertexSize >> 2))
        m_vertexSize  = 4 * (m_vertexSize >> 2) + 4;
    m_fieldMask |= (1 << vertexField);
}

bool VertexFormat::hasField(VertexField vertexField) const
{
    return HasField(m_fieldMask, vertexField);
}

const void* VertexFormat::getFieldOffset(VertexField vertexField, const void *vertexData) const
{
    return (void*)((uintptr_t)vertexData + m_fieldOffset[vertexField]);
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
    return memcmp(this, &other, sizeof(VertexFormat)) < 0;
}

bool VertexFormat::HasField(uint8_t fieldMask, VertexField vertexField)
{
    return (fieldMask >> vertexField) & 1;
}
