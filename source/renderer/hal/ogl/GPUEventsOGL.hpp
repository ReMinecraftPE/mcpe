#pragma once

namespace mce
{
    class GPUEventsOGL
    {
    public:
        void beginProfileSection(char const*);
        void endProfileEvent();
    };
}
