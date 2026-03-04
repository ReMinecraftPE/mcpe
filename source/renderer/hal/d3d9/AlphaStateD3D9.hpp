#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/AlphaStateBase.hpp"

namespace mce
{
    class AlphaStateD3D9 : public AlphaStateBase
    {
    public:
        BOOL m_bAlphaTest;
        D3DCMPFUNC m_alphaFunc;
        uint8_t m_alphaRef;

    public:
        AlphaStateD3D9();

    public:
        void createAlphaState(RenderContext& context, const AlphaStateDescription& desc);
        bool bindAlphaState(RenderContext& context, bool forceBind = false);
    };
}
