#pragma once

#include "renderer/hal/base/BlendStateBase.hpp"

namespace mce
{
    class BlendStateNull : public BlendStateBase
    {
    public:
        bool m_bEnableBlend;

    public:
        BlendStateNull();
    };
}
