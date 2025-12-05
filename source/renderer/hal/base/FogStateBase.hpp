#pragma once

#include "renderer/hal/FogStateDescription.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
	class FogStateBase
	{
	public:
		FogStateDescription m_description;

		FogStateBase();

		void createFogState(RenderContext& context, const FogStateDescription& desc);
		bool bindFogState(RenderContext& context);
	};
}
