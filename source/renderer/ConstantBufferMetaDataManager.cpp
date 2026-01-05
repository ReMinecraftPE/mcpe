#include "thirdparty/rapidjson/document.h"

#include "common/utility/AlignmentHelper.hpp"

#include "ConstantBufferMetaDataManager.hpp"
#include "GlobalConstantBufferManager.hpp"

using namespace mce;

ConstantBufferMetaDataManager::ConstantBufferMetaDataManager()
{
}

void ConstantBufferMetaDataManager::allocateConstantBufferContainers()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    bufferManager.m_constantBufferContainers.reserve(m_constantBufferMetaDataList.size());
    
    for (size_t i = 0; i < m_constantBufferMetaDataList.size(); i++)
    {
        ConstantBufferMetaData& bufferMeta = m_constantBufferMetaDataList[i];
        ConstantBufferContainer buffer;

        unsigned int uniformCount = bufferMeta.m_uniformMetaData.size();
        unsigned int bufferSize = bufferMeta.getRequiredSpaceForUniforms();
        buffer.reserveMemoryForShaderConstants(uniformCount, bufferSize);

        buffer.m_constantBufferName = bufferMeta.getConstantBufferName();

        for (size_t i = 0; i < bufferMeta.m_uniformMetaData.size(); i++)
        {
            UniformMetaData& uniformMeta = bufferMeta.m_uniformMetaData[i];
            buffer.registerReflectedShaderParameter(uniformMeta);
        }

        buffer.finalizeConstantBufferLayout();
        bufferManager.m_constantBufferContainers.push_back(buffer);
    }
}

void ConstantBufferMetaDataManager::loadJsonFile(const std::string& document)
{
    using namespace rapidjson;

    Document d;
    d.Parse(document.c_str());
    const Value& root = d.GetObj();
    m_constantBufferMetaDataList.reserve(root.MemberCount());

    // <root>
    for (Value::ConstMemberIterator it = root.MemberBegin(); it != root.MemberEnd(); it++)
    {
        m_constantBufferMetaDataList.push_back(ConstantBufferMetaData());
        ConstantBufferMetaData& metaData = m_constantBufferMetaDataList.back();
        metaData.m_constantBufferName = it->name.GetString();

        const Value& constantsDef = it->value;

        // "EntityConstants"
        for (Value::ConstValueIterator it2 = constantsDef.Begin(); it2 != constantsDef.End(); it2++)
        {
            Value::ConstMemberIterator uniformDef = it2->MemberBegin();

            ShaderPrimitiveTypes primitiveType = SHADER_PRIMITIVE_UNKNOWN;
            const char* primitiveTypeStr = uniformDef->value.GetString();
            for (int i = 0; i <= SHADER_PRIMITIVES_MAX; i++)
            {
                if (strcmp(ShaderPrimitiveTypeToString[i], primitiveTypeStr) == 0)
                {
                    primitiveType = (ShaderPrimitiveTypes)i;
                    break;
                }
            }
            if (primitiveType == SHADER_PRIMITIVE_UNKNOWN)
            {
                LOG_E("Couldn't find shader primitive type: %s in the array of shader primitive types, typo perhaps?", primitiveTypeStr);
                throw std::bad_cast();
            }
            
            UniformMetaData uniformMeta;
            uniformMeta.m_shaderPrimitiveType = primitiveType;
            uniformMeta.m_numberOfElements = 1;
            uniformMeta.m_constantBufferMetaDataParent = &metaData;
            uniformMeta.m_uniformName = uniformDef->name.GetString();
            metaData.addUniformMetaData(uniformMeta);
        }
    }

    allocateConstantBufferContainers();
}

const UniformMetaData& ConstantBufferMetaDataManager::findUniformMetaData(const std::string& uniformName) const
{
    for (size_t i = 0; i < m_constantBufferMetaDataList.size(); i++)
    {
        const ConstantBufferMetaData& bufferMeta = m_constantBufferMetaDataList[i];
        const UniformMetaData* uniformMeta = bufferMeta.getUniformMetaData(uniformName);
        if (uniformMeta)
            return *uniformMeta;
    }

    LOG_E("Couldn't find uniform: %s in the constant buffer metadata list", uniformName.c_str());
    throw std::bad_cast();
}

const ConstantBufferMetaData& ConstantBufferMetaDataManager::findConstantBuffer(const std::string& bufferName) const
{
    for (size_t i = 0; i < m_constantBufferMetaDataList.size(); i++)
    {
        const ConstantBufferMetaData& bufferMeta = m_constantBufferMetaDataList[i];
        if (bufferMeta.getConstantBufferName() == bufferName)
        {
            return bufferMeta;
        }
    }

    LOG_E("Couldn't find the constant buffer named: %s was it properly reflected in the uniforms metadata file?", bufferName.c_str());
    throw std::bad_cast();
}
