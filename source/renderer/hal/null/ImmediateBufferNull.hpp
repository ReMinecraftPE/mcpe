#pragma once

#include "renderer/hal/base/ImmediateBufferBase.hpp"

namespace mce
{
    class ImmediateBufferNull : public ImmediateBufferBase
    {
    public:
        ImmediateBufferNull();
    };
}