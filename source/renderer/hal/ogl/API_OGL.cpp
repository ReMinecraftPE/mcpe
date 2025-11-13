#include "API_OGL.hpp"

gl::Version* gl::Version::singletonPtr = nullptr;

const gl::Version& gl::Version::singleton()
{
    if (!gl::Version::singletonPtr)
    {
        gl::Version* versionPtr = new gl::Version();
        versionPtr->parse();
        gl::Version::singletonPtr = versionPtr;
    }

    return *singletonPtr;
}

void gl::Version::_findMajorMinor()
{
    major = 0;
    minor = 0;
    core = false;

    const char* versionString = (const char*)glGetString(GL_VERSION);
    if (!versionString)
        return;

    char* endPtr;

    major = strtol(versionString, &endPtr, 10);

    if (*endPtr == '.')
        minor = strtol(endPtr + 1, nullptr, 10);
}

void gl::Version::parse()
{
    _findMajorMinor();

    if (major == 2)
    {
        featureLevel = GLES_2_0;
    }
    else if (major >= 3)
    {
        if (minor == 0)
        {
            featureLevel = GLES_3_0;
        }
        else if (minor == 1)
        {
            featureLevel = GLES_3_1;
        }
        else if (minor >= 2)
        {
            featureLevel = GLES_3_2;
        }
    }
}

std::string gl::getOpenGLVendor()
{
    const GLubyte* glVendorStr = glGetString(GL_VENDOR);
    static std::string glVendor = glVendorStr ? std::string((char*)glVendorStr) : "";
    return glVendor;
}

std::string gl::getOpenGLRenderer()
{
    const GLubyte* glRendererStr = glGetString(GL_RENDERER);
    static std::string glRenderer = glRendererStr ? std::string((char*)glRendererStr) : "";
    return glRenderer;
}

std::string gl::getOpenGLVersion()
{
    const GLubyte* glVersionStr = glGetString(GL_VERSION);
    static std::string glVersion = glVersionStr ? std::string((char*)glVersionStr) : "";
    return glVersion;
}

bool gl::isOpenGLES3()
{
    if (gl::hardwareOverideOpenGLES3)
    {
        return false;
    }

    const gl::Version& glVersion = gl::Version::singleton();

    return glVersion.major >= 3;
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

bool gl::supportsImmediateMode()
{
    return false;

    static int supportsImmediateMode = -1;
    if (supportsImmediateMode < 0)
    {
        const gl::Version& glVersion = gl::Version::singleton();
        supportsImmediateMode = glVersion.core == false;
    }
    return supportsImmediateMode == 1;
}
