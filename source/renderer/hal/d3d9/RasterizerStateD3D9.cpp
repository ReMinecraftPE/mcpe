#include <typeinfo>
#include "RasterizerStateD3D9.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

const D3DCULL cullModeMap[] = {
    /*CULL_NONE*/  D3DCULL_NONE,
    /*CULL_FRONT*/ D3DCULL_CCW,
    /*CULL_BACK*/  D3DCULL_CW
};

RasterizerStateD3D9::RasterizerStateD3D9()
    : RasterizerStateBase()
{
}

void RasterizerStateD3D9::createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc)
{
    RasterizerStateBase::createRasterizerStateDescription(context, desc);

    if ( !context.m_currentState.m_bBoundRasterizerState )
    {
        bindRasterizerState(context, true);
        context.m_currentState.m_rasterizerStateDescription = desc;
        context.m_currentState.m_bBoundRasterizerState = true;
    }
}

bool RasterizerStateD3D9::bindRasterizerState(RenderContext& context, bool forceBind)
{
    RasterizerStateDescription& ctxDesc = context.m_currentState.m_rasterizerStateDescription;
    D3DDevice d3dDevice = context.getD3DDevice();

    // Cull Mode
    if (forceBind || ctxDesc.cullMode != m_description.cullMode)
    {
        d3dDevice->SetRenderState(D3DRS_CULLMODE, cullModeMap[m_description.cullMode]);
        ctxDesc.cullMode = m_description.cullMode;
    }

    // Scissor Test Enable
    if (forceBind || ctxDesc.enableScissorTest != m_description.enableScissorTest)
    {
        d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, m_description.enableScissorTest);
        ctxDesc.enableScissorTest = m_description.enableScissorTest;
    }

    // Depth Bias
    if (forceBind || ctxDesc.depthBias != m_description.depthBias)
    {
        d3dDevice->SetRenderState(D3DRS_DEPTHBIAS, m_description.depthBias);
        d3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, m_description.depthBias);

        ctxDesc.depthBias = m_description.depthBias;
    }

    return RasterizerStateBase::bindRasterizerState(context);
}

void RasterizerStateD3D9::setScissorRect(RenderContext& context, int x, int y, int width, int height)
{
    float viewSizeY = context.m_logicalSize.y;
    
    RECT rect;
    {
        rect.left   = x;
        rect.right  = x + width;
        rect.bottom = (viewSizeY - y);
        rect.top    = (viewSizeY - (y + height));
    }

    D3DDevice d3dDevice = context.getD3DDevice();
    d3dDevice->SetScissorRect(&rect);
}
