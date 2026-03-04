#include "RenderStateD3D9.hpp"

using namespace mce;

RenderStateD3D9::RenderStateD3D9()
{
}

void RenderStateD3D9::createRenderState(RenderContext& context, const RenderStateDescription& desc)
{
    RenderStateBase::createRenderState(context, desc);

    if (!context.m_currentState.m_bBoundRenderState)
    {
        bindRenderState(context, true);
        context.m_currentState.m_bBoundRenderState = true;
        context.m_currentState.m_renderStateDescription = desc;
    }
}

bool RenderStateD3D9::bindRenderState(RenderContext& context, bool forceBind)
{
    RenderStateDescription& ctxDesc = context.m_currentState.m_renderStateDescription;

    D3DDevice d3dDevice = context.getD3DDevice();

    return RenderStateBase::bindRenderState(context);
}
