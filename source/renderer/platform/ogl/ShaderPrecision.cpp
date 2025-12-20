#include <sstream>

#include "ShaderPrecision.hpp"

#include "GameMods.hpp"
#ifdef FEATURE_GFX_SHADERS

#include "common/Util.hpp"

using namespace mce::Platform::OGL;

const std::string Precision::name[3] = {
    "lowp",
    "mediump",
    "highp"
};

const std::string Precision::typeName[3] = {
    "fragment shader",
    "vertex shader",
    "geometry shader"
};

Precision::Precision(GLenum shaderType)
{
    m_precision[0] = _getPrecision(shaderType, GL_LOW_FLOAT);
    m_precision[1] = _getPrecision(shaderType, GL_MEDIUM_FLOAT);
    m_precision[2] = _getPrecision(shaderType, GL_HIGH_FLOAT);
}

GLint Precision::_getPrecision(GLenum shaderType, GLenum precisionType)
{
    GLint range[2];
    GLint precision = -1;

#if GL_VERSION_4_1
    const gl::Version& glVersion = gl::Version::singleton();
    if (glVersion.major >= 4 && glVersion.minor >= 1)
    {
        xglGetShaderPrecisionFormat(shaderType, precisionType, range, &precision);
    }
#endif

    // We only need precision
    if (precision == -1)
    {
        // Default values for GL 4.1
        switch (shaderType)
        {
        case GL_VERTEX_SHADER:
            switch (precisionType)
            {
            case GL_LOW_FLOAT:
                precision = 10;
                break;
            case GL_MEDIUM_FLOAT:
                precision = 10;
                break;
            case GL_HIGH_FLOAT:
                precision = 23;
            }
            break;
        }
    }

    return precision;
}

const std::string& Precision::AtLeast(int atleast)
{
    static Precision info(GL_VERTEX_SHADER);

    if (gl::isOpenGLES())
    {
        for (int i = 0; i < 3; i++)
        {
            if (info.m_precision[i] >= atleast)
            {
                return name[i];
            }
        }
    }

    return Util::EMPTY_STRING;
}

void Precision::BuildHeader(std::ostringstream& stream)
{
    stream << "#define MAT4 " << AtLeast(23) << " mat4\n";
    stream << "#define POS4 " << AtLeast(23) << " vec4\n";
    stream << "#define POS3 " << AtLeast(23) << " vec3\n";
    if (gl::isOpenGLES())
        stream << "precision "    << AtLeast(9)  << " float;\n";
        
}

#endif // FEATURE_GFX_SHADERS