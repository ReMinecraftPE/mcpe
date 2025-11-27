#include "ShaderProgramOGL.hpp"

#ifdef FEATURE_GFX_SHADERS

#include "RenderContextOGL.hpp"

using namespace mce;

const GLenum shaderTypeMap[] = {
    /*SHADER_TYPE_VERTEX*/   GL_VERTEX_SHADER,
    /*SHADER_TYPE_FRAGMENT*/ GL_FRAGMENT_SHADER
};

ShaderProgramOGL::ShaderProgramOGL(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramBase(header, shaderPath, shaderType)
{
    if (shaderSource.empty() || shaderType > SHADER_TYPE_FRAGMENT)
    {
        m_bValid = false;
        return;
    }

    const GLint sourceLength = shaderSource.size();
    const GLchar* sourceStr = (const GLchar*)shaderSource.data();
    m_shaderName = xglCreateShader(shaderTypeMap[shaderType]);
    xglShaderSource(m_shaderName, 1, &sourceStr, &sourceLength);
    xglCompileShader(m_shaderName);

    GLint compileStatus;
    xglGetShaderiv(m_shaderName, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        GLint maxLength = 0;
        xglGetShaderiv(m_shaderName, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 1)
        {
            std::string infoLog(maxLength, 0);
            xglGetShaderInfoLog(m_shaderName, maxLength, &maxLength, (GLchar*)infoLog.data());
            LOG_E("Compiler error:\n %s", infoLog);
        }
        m_bValid = false;
        return;
    }
    
    m_bValid = true;
}

ShaderProgramOGL::~ShaderProgramOGL()
{
    deleteShader();
}

void ShaderProgramOGL::deleteShader()
{
    xglDeleteShader(m_shaderName);
    m_shaderName = GL_NONE;
}

#endif // FEATURE_GFX_SHADERS