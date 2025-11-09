#include <cctype>
#include "API_OGL.hpp"

std::string gl::getOpenGLVendor()
{
    const GLubyte* glVendor = glGetString(GL_VENDOR);
    return glVendor ? std::string((char*)glVendor) : "";
}

std::string gl::getOpenGLRenderer()
{
    const GLubyte* glRenderer = glGetString(GL_RENDERER);
    return glRenderer ? std::string((char*)glRenderer) : "";
}

std::string gl::getOpenGLVersion()
{
    const GLubyte* glVersion = glGetString(GL_VERSION);
    return glVersion ? std::string((char*)glVersion) : "";
}

unsigned int gl::getOpenGLMajorVersion()
{
    std::string version = getOpenGLVersion();
    const char* versionStr = version.c_str();
    while (versionStr[0] != '\0' && !std::isdigit(versionStr[0]))
    {
        versionStr++;
    }
    
    return std::atoi(versionStr);
}

bool gl::isOpenGLES3()
{
    if (gl::hardwareOverideOpenGLES3)
    {
        return false;
    }

    return getOpenGLMajorVersion() >= 3;
}

int gl::getMaxVertexCount()
{
    static int maxVertexCount = 0;
    if (maxVertexCount == 0)
    {
        if (isOpenGLES3())
            maxVertexCount = -1;
        else
            maxVertexCount = 0xFFFF;
    }
    return maxVertexCount;
}

std::string gl::getOpenGLExtensions()
{
    const GLubyte* glExtensions = glGetString(GL_EXTENSIONS);
    return glExtensions ? std::string((char*)glExtensions) : "";
}

bool gl::supportsMipmaps()
{
    static int supportsMipmaps = -1;
    if (supportsMipmaps < 0)
    {
        std::string glExtensions = gl::getOpenGLExtensions();
        if (isOpenGLES3() || glExtensions.find("GL_APPLE_texture_max_level") != std::string::npos)
            supportsMipmaps = 1;
    }
    return supportsMipmaps == 1;
}
