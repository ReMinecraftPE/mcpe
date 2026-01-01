#include <typeinfo>

#include "BlendStateD3D11.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

const D3D11_BLEND blendFuncMap[] = {
    /*BLEND_TARGET_DEST_COLOR*/           D3D11_BLEND_DEST_COLOR,
    /*BLEND_TARGET_SOURCE_COLOR*/         D3D11_BLEND_SRC_COLOR,
    /*BLEND_TARGET_ZERO*/                 D3D11_BLEND_ZERO,
    /*BLEND_TARGET_ONE*/                  D3D11_BLEND_ONE,
    /*BLEND_TARGET_ONE_MINUS_DEST_COLOR*/ D3D11_BLEND_INV_DEST_COLOR,
    /*BLEND_TARGET_ONE_MINUS_SRC_COLOR*/  D3D11_BLEND_INV_SRC_COLOR,
    /*BLEND_TARGET_SOURCE_ALPHA*/         D3D11_BLEND_SRC_ALPHA,
    /*BLEND_TARGET_DEST_ALPHA*/           D3D11_BLEND_DEST_ALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_ALPHA*/  D3D11_BLEND_INV_SRC_ALPHA
};

const D3D11_BLEND blendAlphaFuncMap[] = {
    /*BLEND_TARGET_DEST_COLOR*/           D3D11_BLEND_DEST_ALPHA,
    /*BLEND_TARGET_SOURCE_COLOR*/         D3D11_BLEND_SRC_ALPHA,
    /*BLEND_TARGET_ZERO*/                 D3D11_BLEND_ZERO,
    /*BLEND_TARGET_ONE*/                  D3D11_BLEND_ONE,
    /*BLEND_TARGET_ONE_MINUS_DEST_COLOR*/ D3D11_BLEND_INV_DEST_ALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_COLOR*/  D3D11_BLEND_INV_SRC_ALPHA,
    /*BLEND_TARGET_SOURCE_ALPHA*/         D3D11_BLEND_SRC_ALPHA,
    /*BLEND_TARGET_DEST_ALPHA*/           D3D11_BLEND_DEST_ALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_ALPHA*/  D3D11_BLEND_INV_SRC_ALPHA
};

BlendStateD3D11::BlendStateD3D11()
{
    m_blendFactor[0] = 1.0f;
    m_blendFactor[1] = 1.0f;
    m_blendFactor[2] = 1.0f;
    m_blendFactor[3] = 1.0f;
    m_sampleMask = INT_MAX;
}

void BlendStateD3D11::createBlendState(RenderContext& context, const BlendStateDescription& desc)
{
    BlendStateBase::createBlendState(context, desc);

    CD3D11_BLEND_DESC blendStateDesc(D3D11_DEFAULT);
    {
        BlendTarget blendSource = desc.blendSource;
        BlendTarget blendDestination = desc.blendDestination;

        D3D11_RENDER_TARGET_BLEND_DESC& renderTarget = blendStateDesc.RenderTarget[0];
        renderTarget.RenderTargetWriteMask = desc.colorWriteMask;
        renderTarget.BlendEnable = desc.enableBlend;
        renderTarget.SrcBlend = blendFuncMap[blendSource];
        renderTarget.DestBlend = blendFuncMap[blendDestination];
        renderTarget.SrcBlendAlpha = blendAlphaFuncMap[blendSource];
        renderTarget.DestBlendAlpha = blendAlphaFuncMap[blendDestination];
    }

    m_blendState.release();

    {
        D3DDevice d3dDevice = context.getD3DDevice();
        HRESULT hResult = d3dDevice->CreateBlendState(&blendStateDesc, *m_blendState);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    if (!context.m_currentState.m_bBoundBlendState)
    {
        bindBlendState(context, true);
        context.m_currentState.m_bBoundBlendState = true;
        context.m_currentState.m_blendStateDescription = desc;
    }
}

bool BlendStateD3D11::bindBlendState(RenderContext& context, bool forceBind)
{
    BlendStateDescription& ctxDesc = context.m_currentState.m_blendStateDescription;

    if (forceBind || ctxDesc == m_description)
    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        d3dDeviceContext->OMSetBlendState(**m_blendState, m_blendFactor, m_sampleMask);

        return BlendStateBase::bindBlendState(context);
    }

    return false;
}