#pragma once

#include "renderer/hal/base/DepthStencilStateBase.hpp"

namespace mce
{
    class DepthStencilStateNull : public DepthStencilStateBase
    {
    public:
        DepthStencilStateNull();
    };
}