#include "DepthStencilStateD3D9.hpp"

using namespace mce;

DepthStencilStateD3D9::DepthStencilStateD3D9()
    : DepthStencilStateBase()
{
}

D3DSTENCILOP getStencilOpAction(StencilOp stencilOp)
{
    switch (stencilOp)
    {
    case STENCIL_OP_KEEP:    return D3DSTENCILOP_KEEP;
    case STENCIL_OP_REPLACE: return D3DSTENCILOP_REPLACE;
    default:
        LOG_E("Unknown stencilOp: %d", stencilOp);
        throw std::bad_cast();
    }
}

void DepthStencilStateD3D9::createDepthState(RenderContext& context, const DepthStencilStateDescription& description)
{
    DepthStencilStateBase::createDepthState(context, description);

    if (!context.m_currentState.m_bBoundDepthStencilState)
    {
        bindDepthStencilState(context, true);
        context.m_currentState.m_bBoundDepthStencilState = true;
        context.m_currentState.m_depthStencilStateDescription = description;
    }
}

bool DepthStencilStateD3D9::bindDepthStencilState(RenderContext& context, bool force)
{
    if (!m_description.overwroteStencilRef)
    {
        m_description.stencilRef = context.getStencilReference();
    }

    DepthStencilStateDescription& currentDesc = context.m_currentState.m_depthStencilStateDescription;

    D3DDevice d3dDevice = context.getD3DDevice();

    // Depth Test Enable
    if (force || currentDesc.depthTestEnabled != m_description.depthTestEnabled)
    {
        d3dDevice->SetRenderState(D3DRS_ZENABLE, m_description.depthTestEnabled ? D3DZB_TRUE : D3DZB_FALSE);
        currentDesc.depthTestEnabled = m_description.depthTestEnabled;
    }

    // Depth Write Mask
    if (force || currentDesc.depthWriteMask != m_description.depthWriteMask)
    {
        d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, (m_description.depthWriteMask == DEPTH_WRITE_MASK_ALL));
        currentDesc.depthWriteMask = m_description.depthWriteMask;
    }

    // Depth Function
    if (force || currentDesc.depthFunc != m_description.depthFunc)
    {
        d3dDevice->SetRenderState(D3DRS_ZFUNC, getComparisonFunc(m_description.depthFunc));
        currentDesc.depthFunc = m_description.depthFunc;
    }

    // Stencil Enable
    if (force || currentDesc.stencilTestEnabled != m_description.stencilTestEnabled)
    {
        d3dDevice->SetRenderState(D3DRS_STENCILENABLE, m_description.stencilTestEnabled);
        currentDesc.stencilTestEnabled = m_description.stencilTestEnabled;
    }

    // Stencil Read Mask
    if (force || currentDesc.stencilReadMask != m_description.stencilReadMask)
    {
        d3dDevice->SetRenderState(D3DRS_STENCILMASK, m_description.stencilReadMask);
        currentDesc.stencilReadMask = m_description.stencilReadMask;
    }

    // Stencil Write Mask
    if (force || currentDesc.stencilWriteMask != m_description.stencilWriteMask)
    {
        d3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, m_description.stencilWriteMask);
        currentDesc.stencilWriteMask = m_description.stencilWriteMask;
    }

    // Stencil Ref
    if (force || currentDesc.stencilRef != m_description.stencilRef)
    {
        d3dDevice->SetRenderState(D3DRS_STENCILREF, (DWORD)m_description.stencilRef);
        currentDesc.stencilRef = m_description.stencilRef;
    }

    // Front Face Stencil Operations
    if (force || currentDesc.frontFace != m_description.frontFace)
    {
        d3dDevice->SetRenderState(D3DRS_STENCILFUNC,      getComparisonFunc(m_description.frontFace.stencilFunc));
        d3dDevice->SetRenderState(D3DRS_STENCILPASS,      getStencilOpAction(m_description.frontFace.stencilPassOp));
        d3dDevice->SetRenderState(D3DRS_STENCILFAIL,      getStencilOpAction(m_description.frontFace.stencilFailOp));
        d3dDevice->SetRenderState(D3DRS_STENCILZFAIL,     getStencilOpAction(m_description.frontFace.stencilDepthFailOp));
        currentDesc.frontFace = m_description.frontFace;
    }

    // Back Face Stencil Operations
    if (force || currentDesc.backFace != m_description.backFace)
    {
        d3dDevice->SetRenderState(D3DRS_CCW_STENCILFUNC,  getComparisonFunc(m_description.backFace.stencilFunc));
        d3dDevice->SetRenderState(D3DRS_CCW_STENCILPASS,  getStencilOpAction(m_description.backFace.stencilPassOp));
        d3dDevice->SetRenderState(D3DRS_CCW_STENCILFAIL,  getStencilOpAction(m_description.backFace.stencilFailOp));
        d3dDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, getStencilOpAction(m_description.backFace.stencilDepthFailOp));
        currentDesc.backFace = m_description.backFace;
    }
    
    return DepthStencilStateBase::bindDepthStencilState(context);
}