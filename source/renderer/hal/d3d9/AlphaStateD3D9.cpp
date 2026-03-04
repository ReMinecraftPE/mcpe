#include "AlphaStateD3D9.hpp"

using namespace mce;

AlphaStateD3D9::AlphaStateD3D9()
{
    m_bAlphaTest = FALSE;
    m_alphaFunc = D3DCMP_ALWAYS;
    m_alphaRef = 0;
}

void AlphaStateD3D9::createAlphaState(RenderContext& context, const AlphaStateDescription& desc)
{
    AlphaStateBase::createAlphaState(context, desc);

    m_bAlphaTest = desc.enableAlphaTest ? TRUE : FALSE;
    m_alphaFunc = getComparisonFunc(desc.alphaFunc);
    m_alphaRef = ceilf(desc.alphaRef * 255.0f);

    if (!context.m_currentState.m_bBoundAlphaState)
    {
        bindAlphaState(context, true);
        context.m_currentState.m_bBoundAlphaState = true;
        context.m_currentState.m_alphaStateDescription = desc;
    }
}

bool AlphaStateD3D9::bindAlphaState(RenderContext& context, bool forceBind)
{
    AlphaStateDescription& ctxDesc = context.m_currentState.m_alphaStateDescription;

    D3DDevice d3dDevice = context.getD3DDevice();

    if (forceBind || ctxDesc.enableAlphaTest != m_description.enableAlphaTest)
    {
        d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, m_bAlphaTest);
        ctxDesc.enableAlphaTest = m_description.enableAlphaTest;
    }

    if (forceBind || ctxDesc.alphaFunc != m_description.alphaFunc)
    {
        d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, m_alphaFunc);
        ctxDesc.alphaFunc = m_description.alphaFunc;
    }

    if (forceBind || ctxDesc.alphaRef != m_description.alphaRef)
    {
        d3dDevice->SetRenderState(D3DRS_ALPHAREF, LOWORD(m_alphaRef));
        ctxDesc.alphaRef = m_description.alphaRef;
    }

    return AlphaStateBase::bindAlphaState(context);
}
