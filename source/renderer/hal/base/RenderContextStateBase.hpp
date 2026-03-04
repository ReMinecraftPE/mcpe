#pragma once

#include "renderer/hal/BlendStateDescription.hpp"
#include "renderer/hal/DepthStencilStateDescription.hpp"
#include "renderer/hal/RasterizerStateDescription.hpp"
#include "renderer/hal/RenderStateDescription.hpp"
#include "renderer/hal/AlphaStateDescription.hpp"
#include "renderer/hal/FogStateDescription.hpp"
#include "common/math/Color.hpp"
#include "renderer/hal/enums/ShadeMode.hpp"

namespace mce
{
	// Ranges from 0 - 32768
    typedef uint16_t Gamma;

    class RenderContextStateBase
    {
    public:
        BlendStateDescription m_blendStateDescription;
        DepthStencilStateDescription m_depthStencilStateDescription;
        RasterizerStateDescription m_rasterizerStateDescription;
        RenderStateDescription m_renderStateDescription;
        AlphaStateDescription m_alphaStateDescription;
        FogStateDescription m_fogStateDescription;
        Color m_color;
        ShadeMode m_shadeMode;
        Gamma m_gamma;
        bool m_bBoundBlendState;
        bool m_bBoundDepthStencilState;
        bool m_bBoundRasterizerState;
        bool m_bBoundRenderState;
        bool m_bBoundAlphaState;
        bool m_bBoundFogState;
        bool m_bBoundColor;
        bool m_bBoundShadeMode;
        bool m_bBoundGamma;

    public:
        RenderContextStateBase();
		
	public:
		void clear();
    };
}
