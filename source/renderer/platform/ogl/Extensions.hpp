#pragma once

#include "renderer/hal/ogl/API_OGL.hpp"

namespace mce
{
    namespace Platform
    {
        namespace OGL
        {
            bool InitBindings();
            void* GetProcAddress(const char* name);
#ifdef MC_GL_DEBUG_OUTPUT
            void APIENTRY DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
#endif
        }
    }
}