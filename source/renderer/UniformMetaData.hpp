#pragma once

#include <string>
#include "hal/enums/ShaderPrimitiveTypes.hpp"

namespace mce
{
    class ConstantBufferMetaData;

    class UniformMetaData
    {
    public:
        unsigned int m_numberOfElements;
        unsigned int m_byteOffset;
        ShaderPrimitiveTypes m_shaderPrimitiveType;
        ConstantBufferMetaData* m_constantBufferMetaDataParent;
        std::string m_uniformName;

    public:
        UniformMetaData();

        unsigned int getSize() const;
    };
}
