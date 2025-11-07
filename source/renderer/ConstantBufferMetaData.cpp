#include "common/utility/AlignmentHelper.hpp"
#include "ConstantBufferMetaData.hpp"

using namespace mce;

ConstantBufferMetaData::ConstantBufferMetaData()
{
    m_uniformMetaData = std::vector<UniformMetaData>();
    m_constantBufferName = "";
}

void ConstantBufferMetaData::addUniformMetaData(UniformMetaData& uniformMetaData)
{
    const UniformMetaData& lastUniformMeta = m_uniformMetaData.back();
    uniformMetaData.m_byteOffset = AlignmentHelper::getAlignedOffset(
        lastUniformMeta.m_byteOffset + lastUniformMeta.getSize(),
        ShaderPrimitiveTypeHelper::sizeInBytesFromShaderPrimitiveType(uniformMetaData.m_shaderPrimitiveType)
    );

    m_uniformMetaData.push_back(uniformMetaData);
}

const std::string& ConstantBufferMetaData::getConstantBufferName() const
{
    return m_constantBufferName;
}

const UniformMetaData* ConstantBufferMetaData::getUniformMetaData(const std::string& uniformName) const
{
    for (int i = 0; i < m_uniformMetaData.size(); i++)
    {
        const UniformMetaData& uniformMeta = m_uniformMetaData[i];
        if (uniformMeta.m_uniformName == uniformName)
            return &uniformMeta;
    }

    return nullptr;
}

unsigned int ConstantBufferMetaData::getRequiredSpaceForUniforms() const
{
    if (m_uniformMetaData.empty())
        return 0;
    
    const UniformMetaData& lastUniform = m_uniformMetaData.back();
    unsigned int unalignedSize = lastUniform.getSize() + lastUniform.m_byteOffset;

    return AlignmentHelper::getAlignedOffset(unalignedSize, 16);
}