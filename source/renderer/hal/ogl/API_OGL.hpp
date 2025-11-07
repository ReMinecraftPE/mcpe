#pragma once

#include <string>

#include "thirdparty/GL/GL.hpp"

// This file is actually titled "gl_header.hpp", but I like "API_OGL.hpp" better

namespace gl
{
    extern bool hardwareOverideOpenGLES3;
    
    std::string getOpenGLVendor();
    std::string getOpenGLRenderer();
    std::string getOpenGLVersion();
    unsigned int getOpenGLMajorVersion();
    bool isOpenGLES3();
    int getMaxVertexCount();
    std::string getOpenGLExtensions();
    bool supportsMipmaps();
}