#pragma once

#include "API_D3D11.hpp"
#include "renderer/hal/base/RasterizerStateBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class RasterizerStateD3D11 : public RasterizerStateBase
    {
    private:
        ComInterface<ID3D11RasterizerState> m_rasterizerState;

    public:
        RasterizerStateD3D11();

    public:
        bool bindRasterizerState(RenderContext& context, bool forceBind = false);
        void createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc);
        void setScissorRect(RenderContext &context, int x, int y, int width, int height);
    };
}
