#include "ShaderGroup.hpp"
#include "common/Util.hpp"

using namespace mce;

ShaderGroup* ShaderGroup::singletonPtr = nullptr;

ShaderGroup::ShaderGroup()
     : ShaderGroupBase()
{
}

ShaderGroup::~ShaderGroup()
{

}

void ShaderGroup::_clearShaders()
{
    for (int i = 0; i < m_shaders.size(); i++)
    {
        if (m_shaders[i])
        {
            delete m_shaders[i];
            m_shaders[i] = nullptr;
        }
    }
    m_shaders.clear();
}

void ShaderGroup::_clearPrograms()
{
    for (std::map<std::string, ShaderProgram*>::iterator it = m_programs.begin(); it != m_programs.end(); it++)
    {
        if (it->second)
        {
            delete it->second;
            it->second = nullptr;
        }
    }
    m_programs.clear();
}

void ShaderGroup::onAppSuspended()
{
    _clearShaders();
    _clearPrograms();

    Shader::resetLastProgram();
}

ShaderProgram& ShaderGroup::getShaderProgram(ShaderType shaderType, const std::string& codeOrPath, const std::string& header)
{
    std::string programPath = codeOrPath + header;
    std::map<std::string, ShaderProgram*>::iterator it = m_programs.find(programPath);
    if (it != m_programs.end())
        return *(it->second);

    std::string programCode, shaderPath;
    if (Util::isValidPath(codeOrPath))
    {
        programCode = AppPlatform::singleton()->readAssetFileStr(codeOrPath, true);
        shaderPath = codeOrPath;

        if (shaderType != SHADER_TYPE_GEOMETRY && programCode.empty())
        {
            LOG_E("\nProgram not found: \"%s\"\n\n", codeOrPath.c_str());
        }
    }
    else
    {
        programCode = codeOrPath;
        shaderPath = "";
    }

    if (!programCode.empty())
    {
        programCode.insert(programCode.find('\n') + 1, header);
    }

    ShaderProgram* shaderProgram = new ShaderProgram(shaderType, programCode, programPath, shaderPath);
    ShaderProgram* oldShaderProgram = m_programs[programPath];
    m_programs[programPath] = shaderProgram;
    if (oldShaderProgram)
    {
        delete oldShaderProgram;
    }

    return *shaderProgram;
}

Shader& ShaderGroup::loadShader(const std::string& header, const std::string& vertexCodeOrPath, const std::string& fragmentCodeOrPath, const std::string& geometryCodeOrPath)
{
    for (int i = 0; i < m_shaders.size(); i++)
    {
        Shader* shader = m_shaders[i];
        if (shader->isBuiltFrom(header, vertexCodeOrPath, fragmentCodeOrPath, geometryCodeOrPath))
            return *shader;
    }

    ShaderProgram& vertexProgram   = getShaderProgram(SHADER_TYPE_VERTEX,   vertexCodeOrPath,   header);
    ShaderProgram& fragmentProgram = getShaderProgram(SHADER_TYPE_FRAGMENT, fragmentCodeOrPath, header);
    ShaderProgram& geometryProgram = getShaderProgram(SHADER_TYPE_GEOMETRY, geometryCodeOrPath, header);

    Shader* shader = new Shader(vertexProgram, fragmentProgram, geometryProgram);
    m_shaders.push_back(shader);

    return *shader;
}

ShaderGroup* ShaderGroup::singleton()
{
    if (!singletonPtr)
    {
        singletonPtr = new ShaderGroup();
    }

    return singletonPtr;
}