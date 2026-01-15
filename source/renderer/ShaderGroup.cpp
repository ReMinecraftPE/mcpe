#include "ShaderGroup.hpp"
#include "common/Util.hpp"
//#include <unordered_map>
#include <map>

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
    for (size_t i = 0; i < m_shaders.size(); i++)
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
#if !MCE_GFX_SUPPORTS_INCLUDES
        processIncludeDirectives(codeOrPath, programCode);
#endif

        programCode.insert(0, Util::format("#define %s\n", ShaderTypeToString[shaderType]));
        programCode.insert(programCode.find('\n') + 1, header);

      //  printf("FILE: \n %s \n\n\n\n", programCode.data());
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

void ShaderGroup::processIncludeDirectives(const std::string& path, std::string& code)
{
    std::map<std::string, bool> includeGuards;
    size_t offset = 0;
    while ((offset = code.find("#include")) != std::string::npos)
    {
        size_t openOffset = code.find("\"", offset);
        size_t closeOffset = code.find("\"", openOffset + 1);
        // 4 ... 7 (5, 6, 1, )

        if (openOffset == std::string::npos || closeOffset == std::string::npos || openOffset == closeOffset)
        {
            LOG_E("\nMalformed Include directive: \"%s\"\n\n", path.c_str());
            throw std::bad_cast();
        }

        std::string includeFile = code.substr(openOffset + 1, closeOffset - openOffset - 1);
        includeFile = "shaders/" + includeFile;
        Shader::SpliceShaderPath(includeFile);

        // pragma once
        if (includeGuards[includeFile])
        {
            code.erase(offset, closeOffset - offset + 1);
            continue;
        }

        if (!Util::isValidPath(includeFile))
        {
            LOG_E("\nInvalid shader include: \"%s\"\n\n", path.c_str());
            throw std::bad_cast();
        }

        std::string includeCode = AppPlatform::singleton()->readAssetFileStr(includeFile, true);

        if (includeCode.empty())
        {
            LOG_E("\nProgram not found: \"%s\"\n\n", includeFile.c_str());
            throw std::bad_cast();
        }

        code.erase(offset, closeOffset - offset + 1);
        code.insert(offset, includeCode);
        code.insert(offset, "\n");

        // a xxhash() is nicer for performance concerns, but will do for now as there are less permutations.....
        includeGuards[includeFile] = true;
    }
}

Shader& ShaderGroup::loadShader(const std::string& header, const std::string& vertexCodeOrPath, const std::string& fragmentCodeOrPath, const std::string& geometryCodeOrPath)
{
    for (size_t i = 0; i < m_shaders.size(); i++)
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
