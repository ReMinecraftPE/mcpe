#include <typeinfo>

#include "BlendStateD3D11.hpp"

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
    m_pBlendState = nullptr;
}

void BlendStateD3D11::createBlendState(RenderContext& context, const BlendStateDescription& desc)
{
    BlendStateBase::createBlendState(context, desc);

    CD3D11_BLEND_DESC blendStateDesc;
    {
        BlendTarget blendSource = description->blendSource;
        BlendTarget blendDestination = description->blendDestination;
        const D3D11_RENDER_TARGET_BLEND_DESC& renderTarget = blendStateDesc.RenderTarget[0];

        renderTarget.RenderTargetWriteMask = description->colorWriteMask;
        renderTarget.BlendEnable = description->enableBlend;
        renderTarget.SrcBlend = blendFuncMap[blendSource];
        renderTarget.DestBlend = blendFuncMap[blendDestination];
        renderTarget.SrcBlendAlpha = blendAlphaFuncMap[blendSource];
        renderTarget.DestBlendAlpha = blendAlphaFuncMap[blendDestination];
    }

    if (m_pBlendState)
    {
        m_pBlendState->Release(m_pBlendState);
        m_pBlendState = nullptr;
    }

    ID3D11Device2** GfxDevice; // r0
    HRESULT v11; // r0
    ID3D11Device2* v12; // r3
    ID3D11Device2* v14[3]; // [sp+24h] [bp-234h] BYREF

    GfxDevice = context.getGfxDevice(context);
    v11 = (*GfxDevice)->CreateBlendState(*GfxDevice, &blendStateDesc, &this->m_pBlendState);
    mce::HRESULTToGfxError(v11);
    v12 = v14[0];
    if (v14[0])
    {
        v14[0] = 0;
        v12->Release(v12);
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

    if (forceBind || ctxDesc.enableBlend != m_description.enableBlend)
    {
        if (m_bBlend) glEnable(GL_BLEND);
        else          glDisable(GL_BLEND);
        ctxDesc.enableBlend = m_description.enableBlend;
    }

    if (forceBind || ctxDesc.colorWriteMask != m_description.colorWriteMask)
    {
        glColorMask(m_bRed, m_bGreen, m_bBlue, m_bAlpha);
        ctxDesc.colorWriteMask = m_description.colorWriteMask;
    }

    if (forceBind || ctxDesc.blendSource != m_description.blendSource || ctxDesc.blendDestination != m_description.blendDestination)
    {
        glBlendFunc(m_sfactor, m_dfactor);
        ctxDesc.blendSource = m_description.blendSource;
        ctxDesc.blendDestination = m_description.blendDestination;
    }

    return BlendStateBase::bindBlendState(context); 
}