#pragma once

#include <map>
#include <string>

#include "renderer/Attribute.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"
#include "renderer/hal/interface/RenderDevice.hpp"

namespace mce
{
    class ShaderBase
    {
    public:
        unsigned int m_attributeListIndex;
        ShaderProgram& m_vertexShader;
        ShaderProgram& m_fragmentShader;
        ShaderProgram& m_geometryShader;

        static std::map<std::string, VertexField> builtinAttributeMap;

        ShaderBase(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);

        void finalizeShaderUniforms();
        static void freeCompilerResources();
        static void resetLastProgram();
        bool isBuiltFrom(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) const;
        void validateVertexFormat(const VertexFormat& vertexFormat);
        void bindShader(RenderContext& context, const VertexFormat& format, const void *dataBasePtr, unsigned int shaderStageBits);
        void reflectShader(RenderDevice& renderDevice);
        VertexField getAttributeForName(const std::string& name, unsigned int id) const;
    };
}