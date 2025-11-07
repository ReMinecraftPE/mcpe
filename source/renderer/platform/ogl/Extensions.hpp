#pragma once

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