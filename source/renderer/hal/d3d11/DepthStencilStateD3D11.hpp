#pragma once

#include "API_D3D11.hpp"
#include "renderer/hal/base/DepthStencilStateBase.hpp"

namespace mce
{
    class DepthStencilStateD3D11 : public DepthStencilStateBase
    {
    private:
        ComInterface<ID3D11DepthStencilState> m_depthStencilState;

    public:
        DepthStencilStateD3D11();

    public:
        void createDepthState(RenderContext& context, const DepthStencilStateDescription& description);
        bool bindDepthStencilState(RenderContext& context, bool force = false);
    };
}
