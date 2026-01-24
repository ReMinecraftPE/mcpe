#include "renderer/UniformMetaData.hpp"

#include "ShaderConstantBase.hpp"

using namespace mce;

void ShaderConstantBase::_init()
{
    m_numberOfElements = 0;
    m_byteOffset = 0;
    m_shaderPrimitiveType = SHADER_PRIMITIVE_UNKNOWN;
    m_dirty = false;
}

ShaderConstantBase::ShaderConstantBase(const UniformMetaData& uniMeta)
{
    _init();
    m_name = uniMeta.m_uniformName;
    m_shaderPrimitiveType = uniMeta.m_shaderPrimitiveType;
    m_numberOfElements = uniMeta.m_numberOfElements;
    m_byteOffset = uniMeta.m_byteOffset;
}

ShaderConstantBase::~ShaderConstantBase()
{

}

bool ShaderConstantBase::operator==(const ShaderConstantBase& other) const
{
    return getName() == other.getName() &&
           m_numberOfElements == other.m_numberOfElements &&
           getType() == other.getType() &&
           m_byteOffset == other.m_byteOffset;
}
