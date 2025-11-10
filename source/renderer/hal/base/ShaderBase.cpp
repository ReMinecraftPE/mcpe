#include <sstream>

#include "ShaderBase.hpp"
#include "renderer/RenderContextImmediate.hpp"

using namespace mce;

std::map<std::string, VertexField> ShaderBase::builtinAttributeMap;

ShaderBase::ShaderBase(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : m_vertexShader(vertex)
    , m_fragmentShader(fragment)
    , m_geometryShader(geometry)
{
    m_attributeListIndex = 0;
}

void ShaderBase::finalizeShaderUniforms()
{
}

void ShaderBase::freeCompilerResources()
{
}

void ShaderBase::resetLastProgram()
{
}

bool ShaderBase::isBuiltFrom(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) const
{
    return m_vertexShader.m_header == (shaderName + vertexPath) &&
           m_fragmentShader.m_header == (shaderName + fragmentPath) &&
          (m_geometryShader.m_header == (shaderName + geometryPath) || !m_geometryShader.isValid());
}

void ShaderBase::validateVertexFormat(const VertexFormat& vertexFormat)
{
}

void ShaderBase::bindShader(RenderContext& context, const VertexFormat& format, const void *dataBasePtr, unsigned int shaderStageBits) 
{
}

void ShaderBase::reflectShader(RenderDevice& renderDevice)
{
}

VertexField ShaderBase::getAttributeForName(const std::string& name, unsigned int id) const
{
    std::map<std::string, VertexField>::iterator it = builtinAttributeMap.find(name);

    if (it != builtinAttributeMap.end())
    {
        return it->second;
    }
    else
    {
        std::stringstream ss;
        ss << name << id;
        it = builtinAttributeMap.find(ss.str());

        return it->second;
    }
}