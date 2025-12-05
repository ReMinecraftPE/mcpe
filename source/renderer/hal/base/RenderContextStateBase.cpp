#include "RenderContextStateBase.hpp"

using namespace mce;

RenderContextStateBase::RenderContextStateBase()
{
    m_bBoundBlendState = false;
    m_bBoundDepthStencilState = false;
    m_bBoundRasterizerState = false;
    m_bBoundFixedPipelineState = false;
    m_bBoundFogState = false;
}