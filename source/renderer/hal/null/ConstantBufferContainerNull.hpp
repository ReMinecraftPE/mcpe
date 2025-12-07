#pragma once

#include "renderer/hal/base/ConstantBufferContainerBase.hpp"

namespace mce
{
    class ConstantBufferContainerNull : public ConstantBufferContainerBase
    {
    public:
        ConstantBufferContainerNull();
    };
}