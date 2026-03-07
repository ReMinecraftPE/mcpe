#pragma once

#include "renderer/hal/ogl/API_OGL.hpp"

namespace mce
{
    namespace Platform
    {
        namespace OGL
        {
            extern const char* ERROR_MSG;

            bool InitBindings();
            void* GetProcAddress(const char* name);
        }
    }
}
