#include <cstring>

#include "ShaderProgramOGL.hpp"

#ifdef FEATURE_GFX_SHADERS

#include "RenderContextOGL.hpp"
#include "renderer/hal/ogl/helpers/ErrorHandlerOGL.hpp"

using namespace mce;

#define MULTIVERSION_STRING "// __multiversion__"

const GLenum shaderTypeMap[] = {
    /*SHADER_TYPE_VERTEX*/   GL_VERTEX_SHADER,
    /*SHADER_TYPE_FRAGMENT*/ GL_FRAGMENT_SHADER
};

ShaderProgramOGL::ShaderProgramOGL(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramBase(header, shaderPath, shaderType)
{
    m_shaderName = GL_NONE;

    if (shaderSource.empty() || shaderType > SHADER_TYPE_FRAGMENT)
    {
        m_bValid = false;
        return;
    }
    
    m_bValid = compileShaderProgram(shaderSource);
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

std::string _getVersionMacro()
{
    std::string glslVersion = "140";
    const gl::Version& glVersion = gl::Version::singleton();

    if (glVersion.gles)
    {
        /*
            GLES version  GLSL version
            2.0           1.00 ES
            3.0           3.00 ES
            3.1           3.10 ES
        */
        switch (glVersion.major)
        {
        case 2:
            glslVersion = "100";
            break;
        default:
            glslVersion = "300 es";
            break;
        }
    }
    else
    {
        /*
            GL version  GLSL version
            2.0         1.10
            2.1         1.20
            3.0         1.30
            3.1         1.40
            3.2         1.50
            3.3         3.30
            4.0         4.00
            4.1         4.10
            4.2         4.20
            4.3         4.30
            4.4         4.40
            4.5         4.50
        */
        switch (glVersion.major)
        {
        case 2:
        {
            switch (glVersion.minor)
            {
            case 0: glslVersion = "110"; break;
            case 1: glslVersion = "120"; break;
            }
            break;
        }
        case 3:
        {
            switch (glVersion.minor)
            {
            case 0: glslVersion = "130"; break;
            case 1: glslVersion = "140"; break;
            case 2: glslVersion = "150"; break;
            case 3: glslVersion = "330"; break;
            }
            break;
        }
        case 4:
        {
            switch (glVersion.minor)
            {
            case 0: glslVersion = "400"; break;
            case 1: glslVersion = "410"; break;
            // 4.2 and above support GLSL 4.20 and all versions back to 1.40
            }
            break;
        }
        }
    }

    return "#version " + glslVersion + "\n";
}

bool ShaderProgramOGL::compileShaderProgram(std::string& shaderSource)
{
    ErrorHandlerOGL::checkForErrors();
    
    m_shaderName = xglCreateShader(shaderTypeMap[m_shaderType]);

    if (strncmp(shaderSource.c_str(), MULTIVERSION_STRING, sizeof(MULTIVERSION_STRING) - 1) == 0)
    {
        shaderSource.replace(0, sizeof(MULTIVERSION_STRING), _getVersionMacro());
    }

    const GLint sourceLength = shaderSource.size();
    const GLchar* sourceStr = (const GLchar*)shaderSource.data();
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
            LOG_E("Compiler error:\n %s", infoLog.c_str());
        }
        return false;
    }

    ErrorHandlerOGL::checkForErrors();

    return true;
}

#endif // FEATURE_GFX_SHADERS