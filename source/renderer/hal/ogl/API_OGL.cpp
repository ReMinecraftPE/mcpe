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
#ifdef USE_GLES
    gles = true;
#else
    gles = false;
#endif

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
    static bool fetched = false;
    static std::string glVendor;
    if (!fetched)
    {
        const GLubyte* glVendorStr = glGetString(GL_VENDOR);
        glVendor = glVendorStr ? std::string((char*)glVendorStr) : "";
        fetched = true;
    }
    return glVendor;
}

std::string gl::getOpenGLRenderer()
{
    static bool fetched = false;
    static std::string glRenderer;
    if (!fetched)
    {
        const GLubyte* glRendererStr = glGetString(GL_RENDERER);
        glRenderer = glRendererStr ? std::string((char*)glRendererStr) : "";
        fetched = true;
    }
    return glRenderer;
}

std::string gl::getOpenGLVersion()
{
    static bool fetched = false;
    static std::string glVersion;
    if (!fetched)
    {
        const GLubyte* glVersionStr = glGetString(GL_VERSION);
        glVersion = glVersionStr ? std::string((char*)glVersionStr) : "";
        fetched = true;
    }
    return glVersion;
}

bool gl::isOpenGLES()
{
    const gl::Version& glVersion = gl::Version::singleton();
    
    return glVersion.gles;
}

bool gl::isOpenGLES3()
{
    if (gl::hardwareOverideOpenGLES3)
    {
        return false;
    }

    const gl::Version& glVersion = gl::Version::singleton();

    return glVersion.gles && glVersion.major >= 3;
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

const std::string& gl::getOpenGLExtensions()
{
    static bool fetched = false;
    static std::string glExtensions;
    if (!fetched)
    {
        const GLubyte* glExtensionsStr = glGetString(GL_EXTENSIONS);
        glExtensions = glExtensionsStr ? std::string((char*)glExtensionsStr) : "";
    }
    return glExtensions;
}

bool gl::supportsMipmaps()
{
    static int supportsMipmaps = -1;
    if (supportsMipmaps < 0)
    {
        const std::string& glExtensions = gl::getOpenGLExtensions();
        if (isOpenGLES3() || glExtensions.find("GL_APPLE_texture_max_level") != std::string::npos)
            supportsMipmaps = 1;
    }
    return supportsMipmaps == 1;
}

bool gl::supportsImmediateMode()
{
#ifndef FEATURE_GFX_SHADERS
    return false;
#endif

    static int supportsImmediateMode = -1;
    if (supportsImmediateMode < 0)
    {
        const gl::Version& glVersion = gl::Version::singleton();
        supportsImmediateMode = glVersion.core == false;
    }
    return supportsImmediateMode == 1;
}

bool gl::supports32BitIndices()
{
    static int isSupported = -1;
    if (isSupported < 0)
    {
        const std::string& glExtensions = gl::getOpenGLExtensions();
        if (isOpenGLES3() || glExtensions.find("GL_OES_element_index") != std::string::npos)
            isSupported = 1;
    }
    return isSupported == 1;
}
