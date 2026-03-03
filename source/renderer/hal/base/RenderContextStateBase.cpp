#include "RenderContextStateBase.hpp"

using namespace mce;

RenderContextStateBase::RenderContextStateBase()
{
    m_shadeMode = SHADE_MODE_SMOOTH;
	clear();
}

void RenderContextStateBase::clear()
{
    m_bBoundBlendState = false;
    m_bBoundDepthStencilState = false;
    m_bBoundRasterizerState = false;
    m_bBoundFixedPipelineState = false;
    m_bBoundFogState = false;
    m_bBoundColor = false;
    m_bBoundShadeMode = false;
    m_bBoundGamma = false;
}
