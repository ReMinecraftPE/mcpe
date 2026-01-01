#include "DepthStencilStateD3D11.hpp"

using namespace mce;

DepthStencilStateD3D11::DepthStencilStateD3D11()
    : DepthStencilStateBase()
{
}

D3D11_STENCIL_OP getStencilOpAction(StencilOp stencilOp)
{
    switch (stencilOp)
    {
    case STENCIL_OP_KEEP:    return D3D11_STENCIL_OP_KEEP;
    case STENCIL_OP_REPLACE: return D3D11_STENCIL_OP_REPLACE;
    default:
        LOG_E("Unknown stencilOp: %d", stencilOp);
        throw std::bad_cast();
    }
}

D3D11_DEPTH_WRITE_MASK getDepthWriteMask(DepthWriteMask depthWriteMask)
{
    switch (depthWriteMask)
    {
    case DEPTH_WRITE_MASK_NONE: return D3D11_DEPTH_WRITE_MASK_ZERO;
    case DEPTH_WRITE_MASK_ALL:  return D3D11_DEPTH_WRITE_MASK_ALL;
    default:
        LOG_E("Unknown depthWriteMask: %d", depthWriteMask);
        throw std::bad_cast();
    }
}

D3D11_DEPTH_STENCILOP_DESC getStencilOpDesc(const StencilFaceDescription& faceDesc)
{
    D3D11_DEPTH_STENCILOP_DESC opDesc;

    opDesc.StencilFunc = getComparisonFunc(faceDesc.stencilFunc);
    opDesc.StencilPassOp = getStencilOpAction(faceDesc.stencilPassOp);
    opDesc.StencilFailOp = getStencilOpAction(faceDesc.stencilFailOp);
    opDesc.StencilDepthFailOp = getStencilOpAction(faceDesc.stencilDepthFailOp);

    return opDesc;
}

void DepthStencilStateD3D11::createDepthState(RenderContext& context, const DepthStencilStateDescription& description)
{
    DepthStencilStateBase::createDepthState(context, description);

    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    {
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.DepthEnable = description.depthTestEnabled;

        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.DepthWriteMask = getDepthWriteMask(description.depthWriteMask);

        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.StencilWriteMask = description.stencilWriteMask;

        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.StencilReadMask = description.stencilReadMask;

        depthStencilDesc.DepthFunc = getComparisonFunc(description.depthFunc);
        depthStencilDesc.StencilEnable = description.stencilTestEnabled;
        depthStencilDesc.FrontFace = getStencilOpDesc(description.frontFace);
        depthStencilDesc.BackFace = getStencilOpDesc(description.backFace);
    }

    m_depthStencilState.release();

    {
        D3DDevice d3dDevice = context.getD3DDevice();
        d3dDevice->CreateDepthStencilState(&depthStencilDesc, *m_depthStencilState);
    }

    if (!context.m_currentState.m_bBoundDepthStencilState)
    {
        bindDepthStencilState(context, true);
        context.m_currentState.m_depthStencilStateDescription.depthTestEnabled = description.depthTestEnabled;
        context.m_currentState.m_depthStencilStateDescription.frontFace.stencilDepthFailOp = description.frontFace.stencilDepthFailOp;
        context.m_currentState.m_depthStencilStateDescription.backFace.stencilDepthFailOp = description.backFace.stencilDepthFailOp;
        context.m_currentState.m_depthStencilStateDescription.stencilReadMask = description.stencilReadMask;
        context.m_currentState.m_depthStencilStateDescription.stencilWriteMask = description.stencilWriteMask;
        context.m_currentState.m_depthStencilStateDescription.stencilRef = description.stencilRef;
        context.m_currentState.m_depthStencilStateDescription.overwroteStencilRef = description.overwroteStencilRef;
        context.m_currentState.m_bBoundDepthStencilState = true;
    }
}

bool DepthStencilStateD3D11::bindDepthStencilState(RenderContext& context, bool force)
{
    if (!m_description.overwroteStencilRef)
    {
        m_description.stencilRef = context.getStencilReference();
    }

    DepthStencilStateDescription& currentDesc = context.m_currentState.m_depthStencilStateDescription;

    if (force || currentDesc != m_description)
    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        d3dDeviceContext->OMSetDepthStencilState(**m_depthStencilState,m_description.stencilRef);
        currentDesc = m_description;
    }

    // Doesn't exist, do we need it like we do in OGL?
    //currentDesc.overwroteStencilRef = m_description.overwroteStencilRef;
    
    return DepthStencilStateBase::bindDepthStencilState(context);
}