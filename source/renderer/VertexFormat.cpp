#include <string.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <assert.h>

#include "VertexFormat.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"
#include "VertexFieldFormat.hpp"
#include "GameMods.hpp"

using namespace mce;

const VertexFormat VertexFormat::EMPTY = VertexFormat();

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
    m_vertexSize += VertexFieldFormats::GetFormatByField(vertexField).size;
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
    assert(m_fieldOffset[vertexField] != UINT8_MAX);
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
