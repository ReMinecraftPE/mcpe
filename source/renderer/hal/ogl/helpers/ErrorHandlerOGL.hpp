#pragma once

#include "../API_OGL.hpp"

namespace mce
{
    class ErrorHandlerOGL
    {
    public:
        static void checkForErrors();
#ifdef MC_GL_DEBUG_OUTPUT
		static void debugMessageCallbackOpenGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
#endif
    };
}
