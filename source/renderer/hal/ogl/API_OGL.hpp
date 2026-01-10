#pragma once

#include <string>

#include "thirdparty/GL/GL.hpp"
#include "compat/Unused.hpp"

// This file is actually titled "gl_header.h", but I like "API_OGL.hpp" better

namespace gl
{
    class Version
    {
    public:
        enum GL_FEATURE_LEVEL
        {
            GLES_2_0,
            GLES_3_0,
            GLES_3_1,
            GLES_3_2
        };

    private:
        static Version* singletonPtr;
    public:
        static const Version& singleton();

    public:
        int major;
        int minor;
        GL_FEATURE_LEVEL featureLevel;
        bool core;
        bool gles;

    private:
        void _findMajorMinor();
    public:
        void parse();
    };

    MC_UNUSED static bool hardwareOverideOpenGLES3 = false;
    
    std::string getOpenGLVendor();
    std::string getOpenGLRenderer();
    std::string getOpenGLVersion();
    bool isOpenGLES();
    bool isOpenGLES3();
    int getMaxVertexCount();
    const std::string& getOpenGLExtensions();
    bool supportsMipmaps();
    bool supportsImmediateMode();
    bool supports32BitIndices();
    bool supports16BitUnsignedUVs();
}
