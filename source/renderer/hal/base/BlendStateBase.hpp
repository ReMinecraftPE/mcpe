#pragma once

#include "renderer/hal/BlendStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
	class BlendStateBase
	{
	public:
        BlendStateDescription m_description;
		
		BlendStateBase();
		
		void createBlendState(RenderContext& context, const BlendStateDescription& desc);
        bool bindBlendState(RenderContext& context);
	};
}
