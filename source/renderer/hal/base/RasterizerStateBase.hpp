#pragma once

#include "renderer/hal/RasterizerStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
	class RasterizerStateBase
	{
	public:
        RasterizerStateDescription m_description;
		
		RasterizerStateBase();
		
		void createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc);
        bool bindRasterizerState(RenderContext& context);
		void setScissorRect(RenderContext &context, int x, int y, int width, int height) {}
	};
}
