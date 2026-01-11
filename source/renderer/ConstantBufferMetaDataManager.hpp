#pragma once

#include <vector>

#include "common/utility/Singleton.hpp"
#include "ConstantBufferMetaData.hpp"

namespace mce
{
    class ConstantBufferMetaDataManager : public Singleton<ConstantBufferMetaDataManager>
    {
    private:
        std::vector<ConstantBufferMetaData> m_constantBufferMetaDataList;

    public:
        ConstantBufferMetaDataManager();

    public:
        void allocateConstantBufferContainers();
        void loadJsonFile(const std::string& document);
        const UniformMetaData* tryFindUniformMetaData(const std::string& uniformName) const;
        const UniformMetaData& findUniformMetaData(const std::string& uniformName) const;
        const ConstantBufferMetaData& findConstantBuffer(const std::string& bufferName) const;
    };
}