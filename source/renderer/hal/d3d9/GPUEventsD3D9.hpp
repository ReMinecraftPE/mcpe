#pragma once

namespace mce
{
    class GPUEventsD3D9
    {
    public:
        void beginProfileSection(char const*);
        void endProfileEvent();
    };
}