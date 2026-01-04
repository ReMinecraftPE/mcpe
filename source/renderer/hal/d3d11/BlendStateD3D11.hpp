#pragma once

#include "API_D3D11.hpp"
#include "renderer/hal/base/BlendStateBase.hpp"

namespace mce
{
    class BlendStateD3D11 : public BlendStateBase
    {
    public:
        FLOAT m_blendFactor[4];
        UINT m_sampleMask;
        ComInterface<ID3D11BlendState> m_blendState;

    public:
        BlendStateD3D11();

    public:
		void createBlendState(RenderContext& context, const BlendStateDescription& desc);
        bool bindBlendState(RenderContext& context, bool forceBind = false);
    };
}