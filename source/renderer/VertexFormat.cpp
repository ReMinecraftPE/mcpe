#include <string.h>

#include "VertexFormat.hpp"

using namespace mce;

const VertexFormat VertexFormat::EMPTY = VertexFormat();
const unsigned int VertexFormat::FieldSize[8] = { 0xC, 4, 4, 4, 4, 0, 0, 0 };

VertexFormat::VertexFormat()
{
    m_fieldMask = 0;
    m_vertexSize = 0;
    memset(m_fieldOffset, -1, sizeof(m_fieldOffset));
}

void VertexFormat::enableField(VertexField vertexField)
{
    if (hasField(vertexField)) return;

    m_fieldOffset[vertexField] = m_vertexSize;
    uint8_t v6 = m_vertexSize + VertexFormat::FieldSize[vertexField];
    uint8_t v8 = v6 & 3;
    m_vertexSize = v6;
    if (v8)
        m_vertexSize += 4 - v8;
    m_fieldMask = (1 << vertexField) | m_fieldMask;
}

bool VertexFormat::hasField(VertexField vertexField) const
{
    return (m_fieldMask >> vertexField) & 1;
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
