#pragma once

#include <string>
#include <vector>
#include "UniformMetaData.hpp"

namespace mce
{
    class ConstantBufferMetaData
    {
    public:
        std::string m_constantBufferName;
        std::vector<UniformMetaData> m_uniformMetaData;

    public:
        ConstantBufferMetaData();

    public:
        // doesn't exist in 0.12.1, assuming it's in-lined
        void addUniformMetaData(UniformMetaData& uniformMetaData);

        const std::string& getConstantBufferName() const;
        const UniformMetaData* getUniformMetaData(const std::string& uniformName) const;
        unsigned int getRequiredSpaceForUniforms() const;
    };
}
