#pragma once

#include "hal/interface/RenderContext.hpp"

namespace mce
{
    class RenderContextImmediate
    {
    public:
        static RenderContext& get();
        static const RenderContext& getAsConst();
    };
}