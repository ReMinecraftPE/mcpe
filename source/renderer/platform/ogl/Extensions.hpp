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
        }
    }
}
