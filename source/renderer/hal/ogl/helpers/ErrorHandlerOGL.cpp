#include "ErrorHandlerOGL.hpp"
#include "common/Logger.hpp"

using namespace mce;

void ErrorHandlerOGL::checkForErrors()
{
#ifdef _DEBUG
	GLenum __err = glGetError();
	if (__err != GL_NO_ERROR)
	{
		LOG_E("OpenGL Error: 0x%X", __err);
		throw std::bad_cast();
	}
#endif
}

#ifdef MC_GL_DEBUG_OUTPUT
void APIENTRY ErrorHandlerOGL::debugMessageCallbackOpenGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	LOG_I("GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n", type, severity, message);
}
#endif
