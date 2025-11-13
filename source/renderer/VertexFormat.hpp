#pragma once

#include <stdint.h>

#include "hal/enums/VertexField.hpp"

namespace mce
{
    class VertexFormat
    {
        struct FieldOffset
        {
            uint8_t m_offset;
            uint8_t m_size;
        };

    public:
        static const VertexFormat EMPTY, VT, VTC, VTN;
        static const unsigned int FieldSize[VERTEX_FIELDS_COUNT];

    private:
        uint8_t m_fieldMask;
        uint8_t m_fieldOffset[5];
        uint8_t m_vertexSize;

    private:
        void _init();

    public:
        VertexFormat();
        VertexFormat(uint8_t fieldMask);

    public:
        void enableField(VertexField vertexField);

        bool hasField(VertexField vertexField) const;
        const void* getFieldOffset(VertexField vertexField, const void *vertexData = nullptr) const;

        unsigned int getID() const { return m_fieldMask; }
        unsigned int getVertexSize() const { return m_vertexSize; }

        bool operator==(const VertexFormat &other) const;
        bool operator!=(const VertexFormat &other) const;
        bool operator<(const VertexFormat &other) const;
        operator bool() const { return *this != VertexFormat::EMPTY; }

        static bool HasField(uint8_t fieldMask, VertexField vertexField);
    };
}