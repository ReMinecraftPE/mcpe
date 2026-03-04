#pragma once

#include "enums/ComparisonFunc.hpp"

namespace mce
{
	struct RenderStateDescription
	{
		bool enableTexture;

		RenderStateDescription();

		bool operator==(const RenderStateDescription& other) const;
		bool operator!=(const RenderStateDescription& other) const;
	};
}
