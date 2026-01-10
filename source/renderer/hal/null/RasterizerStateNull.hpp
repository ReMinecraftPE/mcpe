#pragma once

#include "renderer/hal/base/RasterizerStateBase.hpp"

namespace mce
{
    class RasterizerStateNull : public RasterizerStateBase
    {
    public:
        RasterizerStateNull();
    };
}
