#pragma once

#include "renderer/hal/base/RenderContextBase.hpp"

namespace mce
{
    class RenderContextNull : public RenderContextBase
    {
    public:
        RenderContextNull();
    };
}
