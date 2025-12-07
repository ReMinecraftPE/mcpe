#include "UniformMetaData.hpp"
#include "compat/LegacyCPP.hpp"

using namespace mce;

UniformMetaData::UniformMetaData()
{
    m_numberOfElements = 1;
    m_byteOffset = 0;
    m_shaderPrimitiveType = SHADER_PRIMITIVE_UNKNOWN;
    m_constantBufferMetaDataParent = nullptr;
}

unsigned int UniformMetaData::getSize() const
{
    return ShaderPrimitiveTypeHelper::sizeInBytesFromShaderPrimitiveType(m_shaderPrimitiveType);
}