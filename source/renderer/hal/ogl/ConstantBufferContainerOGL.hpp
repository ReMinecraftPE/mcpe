#pragma once

#include "renderer/hal/base/ConstantBufferContainerBase.hpp"

namespace mce
{
    class ConstantBufferContainerOGL : public ConstantBufferContainerBase
    {
    public:
        ConstantBufferContainerOGL();

        void sync(RenderContext& context);
    };
}