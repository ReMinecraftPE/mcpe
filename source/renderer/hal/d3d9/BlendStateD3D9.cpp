#include <typeinfo>

#include "BlendStateD3D9.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

const D3DBLEND blendFuncMap[] = {
    /*BLEND_TARGET_DEST_COLOR*/           D3DBLEND_DESTCOLOR,
    /*BLEND_TARGET_SOURCE_COLOR*/         D3DBLEND_SRCCOLOR,
    /*BLEND_TARGET_ZERO*/                 D3DBLEND_ZERO,
    /*BLEND_TARGET_ONE*/                  D3DBLEND_ONE,
    /*BLEND_TARGET_ONE_MINUS_DEST_COLOR*/ D3DBLEND_INVDESTCOLOR,
    /*BLEND_TARGET_ONE_MINUS_SRC_COLOR*/  D3DBLEND_INVSRCCOLOR,
    /*BLEND_TARGET_SOURCE_ALPHA*/         D3DBLEND_SRCALPHA,
    /*BLEND_TARGET_DEST_ALPHA*/           D3DBLEND_DESTALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_ALPHA*/  D3DBLEND_INVSRCALPHA
};

const D3DBLEND blendAlphaFuncMap[] = {
    /*BLEND_TARGET_DEST_COLOR*/           D3DBLEND_DESTALPHA,
    /*BLEND_TARGET_SOURCE_COLOR*/         D3DBLEND_SRCALPHA,
    /*BLEND_TARGET_ZERO*/                 D3DBLEND_ZERO,
    /*BLEND_TARGET_ONE*/                  D3DBLEND_ONE,
    /*BLEND_TARGET_ONE_MINUS_DEST_COLOR*/ D3DBLEND_INVDESTALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_COLOR*/  D3DBLEND_INVSRCALPHA,
    /*BLEND_TARGET_SOURCE_ALPHA*/         D3DBLEND_SRCALPHA,
    /*BLEND_TARGET_DEST_ALPHA*/           D3DBLEND_DESTALPHA,
    /*BLEND_TARGET_ONE_MINUS_SRC_ALPHA*/  D3DBLEND_INVSRCALPHA
};

BlendStateD3D9::BlendStateD3D9()
{
#if MCE_GFX_D3D9_BLENDSTATE
    memset(&m_blendState, 0, sizeof(m_blendState));
    // These don't change
    m_blendState.BlendOp      = D3DBLENDOP_ADD;
    m_blendState.BlendOpAlpha = D3DBLENDOP_ADD;
#endif
}

void BlendStateD3D9::createBlendState(RenderContext& context, const BlendStateDescription& desc)
{
    BlendStateBase::createBlendState(context, desc);

#if MCE_GFX_D3D9_BLENDSTATE
    if (desc.enableBlend)
    {
        m_blendState.SrcBlend       = blendFuncMap[m_description.blendSource];
        m_blendState.DestBlend      = blendFuncMap[m_description.blendDestination];
        m_blendState.SrcBlendAlpha  = blendAlphaFuncMap[m_description.blendSource];
        m_blendState.DestBlendAlpha = blendAlphaFuncMap[m_description.blendDestination];
    }
    else
    {
        m_blendState.SrcBlend       = D3DBLEND_ONE;
        m_blendState.DestBlend      = D3DBLEND_ZERO;
        m_blendState.SrcBlendAlpha  = D3DBLEND_ONE;
        m_blendState.DestBlendAlpha = D3DBLEND_ZERO;
    }
#endif

    if (!context.m_currentState.m_bBoundBlendState)
    {
        bindBlendState(context, true);
        context.m_currentState.m_bBoundBlendState = true;
        context.m_currentState.m_blendStateDescription = desc;
    }
}

bool BlendStateD3D9::bindBlendState(RenderContext& context, bool forceBind)
{
    BlendStateDescription& ctxDesc = context.m_currentState.m_blendStateDescription;

    D3DDevice d3dDevice = context.getD3DDevice();

#if MCE_GFX_D3D9_BLENDSTATE
    if (forceBind || ctxDesc != m_description)
    {
        d3dDevice->SetBlendState(0, m_blendState);
		ctxDesc = m_description;
    }
#else
    if (forceBind || ctxDesc.colorWriteMask != m_description.colorWriteMask)
    {
        // Our ColorWriteMask is identical to D3D's
        d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, m_description.colorWriteMask);
        ctxDesc.colorWriteMask = m_description.colorWriteMask;
    }

    if (forceBind || ctxDesc.enableBlend != m_description.enableBlend)
    {
        d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_description.enableBlend);
        ctxDesc.enableBlend = m_description.enableBlend;
    }

    if (forceBind || ctxDesc.blendSource != m_description.blendSource)
    {
        d3dDevice->SetRenderState(D3DRS_SRCBLEND, blendFuncMap[m_description.blendSource]);
        d3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, blendAlphaFuncMap[m_description.blendSource]);
        ctxDesc.blendSource = m_description.blendSource;
    }

    if (forceBind || ctxDesc.blendDestination != m_description.blendDestination)
    {
        d3dDevice->SetRenderState(D3DRS_DESTBLEND, blendFuncMap[m_description.blendDestination]);
        d3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, blendAlphaFuncMap[m_description.blendDestination]);
        ctxDesc.blendDestination = m_description.blendDestination;
    }
#endif

    return BlendStateBase::bindBlendState(context);
}