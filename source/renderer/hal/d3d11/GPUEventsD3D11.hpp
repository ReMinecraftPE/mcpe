#pragma once

namespace mce
{
    class GPUEventsD3D11
    {
    public:
        void beginProfileSection(char const*);
        void endProfileEvent();
    };
}
