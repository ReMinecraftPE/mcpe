#pragma once

#include <string>

#include "thirdparty/GL/GL.hpp"

// This file is actually titled "gl_header.hpp", but I like "API_OGL.hpp" better

namespace gl
{
    static bool hardwareOverideOpenGLES3 = false;
    
    std::string getOpenGLVendor();
    std::string getOpenGLRenderer();
    std::string getOpenGLVersion();
    unsigned int getOpenGLMajorVersion();
    bool isOpenGLES3();
    int getMaxVertexCount();
    std::string getOpenGLExtensions();
    bool supportsMipmaps();
}