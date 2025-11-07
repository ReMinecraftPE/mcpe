#pragma once

#include "renderer/hal/DepthStencilStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
	class DepthStencilStateBase
	{
	public:
        DepthStencilStateDescription m_description;
		
	public:
		DepthStencilStateBase();
		
	public:
		void createDepthState(RenderContext& context, const DepthStencilStateDescription& description);
        bool bindDepthStencilState(RenderContext& context);
	};
}
