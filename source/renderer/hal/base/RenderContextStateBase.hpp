#pragma once

#include "renderer/hal/BlendStateDescription.hpp"
#include "renderer/hal/DepthStencilStateDescription.hpp"
#include "renderer/hal/RasterizerStateDescription.hpp"
#include "renderer/hal/FixedPipelineStateDescription.hpp"
#include "renderer/hal/FogStateDescription.hpp"

namespace mce
{
    class RenderContextStateBase
    {
    public:
        BlendStateDescription m_blendStateDescription;
        DepthStencilStateDescription m_depthStencilStateDescription;
        RasterizerStateDescription m_rasterizerStateDescription;
        FixedPipelineStateDescription m_fixedPipelineStateDescription;
        FogStateDescription m_fogStateDescription;
        bool m_bBoundBlendState;
        bool m_bBoundDepthStencilState;
        bool m_bBoundRasterizerState;
        bool m_bBoundFixedPipelineState;
        bool m_bBoundFogState;

    public:
        RenderContextStateBase();
		
	public:
		void clear();
    };
}
