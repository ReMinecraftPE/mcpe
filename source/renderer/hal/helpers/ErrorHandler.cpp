#include "ErrorHandler.hpp"
#include "renderer/PlatformDefinitions.h"
#include "common/Logger.hpp"

#if MCE_GFX_API_OGL
#include "renderer/hal/ogl/API_OGL.hpp"
#endif

using namespace mce;

void ErrorHandler::checkForErrors()
{
#ifdef _DEBUG
#if MCE_GFX_API_OGL
	GLenum __err = glGetError();
	if (__err != GL_NO_ERROR)
	{
		LOG_E("OpenGL Error: %s", gluErrorString(__err));
		throw std::bad_cast();
	}
#endif
#endif
}