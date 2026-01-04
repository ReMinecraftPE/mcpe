#include <typeinfo>
#include "RasterizerStateD3D11.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

const D3D11_CULL_MODE cullModeMap[] = {
    /*CULL_NONE*/  D3D11_CULL_NONE,
    /*CULL_FRONT*/ D3D11_CULL_FRONT,
    /*CULL_BACK*/  D3D11_CULL_BACK
};

RasterizerStateD3D11::RasterizerStateD3D11()
    : RasterizerStateBase()
{
}

bool RasterizerStateD3D11::bindRasterizerState(RenderContext& context, bool forceBind)
{
    RasterizerStateDescription& ctxDesc = context.m_currentState.m_rasterizerStateDescription;

    if (forceBind || ctxDesc != m_description)
    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        d3dDeviceContext->RSSetState(**m_rasterizerState);
        ctxDesc = m_description;
    }

    return RasterizerStateBase::bindRasterizerState(context);
}

void RasterizerStateD3D11::createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc)
{
    RasterizerStateBase::createRasterizerStateDescription(context, desc);

    CD3D11_RASTERIZER_DESC rasterizerDesc;
    {
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.DepthBiasClamp = 0.0f;

        rasterizerDesc.CullMode = cullModeMap[desc.cullMode];
        rasterizerDesc.DepthBias = 1000 * (int)desc.depthBias; //ceilf(desc.depthBias * 1000.0f);

        rasterizerDesc.FrontCounterClockwise = true; // Mojang had this set to false in WP 0.12.1
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        rasterizerDesc.ScissorEnable = desc.enableScissorTest;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.SlopeScaledDepthBias = desc.depthBias;
    }

    {
        D3DDevice d3dDevice = context.getD3DDevice();
        HRESULT hResult = d3dDevice->CreateRasterizerState(&rasterizerDesc, *m_rasterizerState);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    if ( !context.m_currentState.m_bBoundRasterizerState )
    {
        bindRasterizerState(context, true);
        context.m_currentState.m_rasterizerStateDescription = desc;
        context.m_currentState.m_bBoundRasterizerState = true;
    }
}

void RasterizerStateD3D11::setScissorRect(RenderContext& context, int x, int y, int width, int height)
{
    float viewSizeY = context.m_logicalSize.y;
    
    D3D11_RECT rect;
    {
        rect.left   = x;
        rect.right  = x + width;
        rect.bottom = (viewSizeY - y);
        rect.top    = (viewSizeY - (y + height));
    }

    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    d3dDeviceContext->RSSetScissorRects(1, &rect);
}
