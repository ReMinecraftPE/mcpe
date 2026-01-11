#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/BlendStateBase.hpp"

namespace mce
{
    class BlendStateD3D9 : public BlendStateBase
    {
    public:
        BlendStateD3D9();

    public:
		void createBlendState(RenderContext& context, const BlendStateDescription& desc);
        bool bindBlendState(RenderContext& context, bool forceBind = false);
    };
}