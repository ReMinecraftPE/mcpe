#include <sstream>

#include "ShaderBase.hpp"
#include "renderer/RenderContextImmediate.hpp"

using namespace mce;

// I could not find anything responsible for setting this in 0.12.1
std::map<std::string, VertexField> _CreateBuiltinAttributeMap()
{
    std::map<std::string, VertexField> m;
    m["POSITION"]   = VERTEX_FIELD_POSITION;
    m["COLOR"]      = VERTEX_FIELD_COLOR;
    m["NORMAL"]     = VERTEX_FIELD_NORMAL;
    m["TEXCOORD_0"] = VERTEX_FIELD_UV0;
    m["TEXCOORD_1"] = VERTEX_FIELD_UV1;
    return m;
}
std::map<std::string, VertexField> ShaderBase::builtinAttributeMap = _CreateBuiltinAttributeMap();

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

void ShaderBase::compileAndLinkShader()
{
}

void ShaderBase::reflectShader(RenderDevice& renderDevice)
{
}

VertexField ShaderBase::getAttributeForName(const std::string& name, unsigned int semanticIndex) const
{
    std::map<std::string, VertexField>::iterator it = builtinAttributeMap.find(name);

    if (it != builtinAttributeMap.end())
    {
        return it->second;
    }
    else
    {
        std::stringstream ss;
        ss << name << semanticIndex;
        it = builtinAttributeMap.find(ss.str());

        return it->second;
    }
}

void ShaderBase::SpliceShaderPath(std::string& shaderName, const std::string& shaderDir, const std::string& shaderFileExtension)
{
    size_t shaderPathPos = shaderName.find_first_not_of("shaders");
    if (shaderPathPos != std::string::npos && shaderName.find(shaderDir) == std::string::npos)
    {
        shaderName.insert(shaderPathPos, shaderDir);
        shaderName.append(shaderFileExtension);
    }
}

void ShaderBase::SpliceShaderPath(std::string& shaderName)
{
}

void ShaderBase::BuildHeader(std::ostringstream& stream)
{
}