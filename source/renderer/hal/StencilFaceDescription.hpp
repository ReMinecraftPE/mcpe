#pragma once

#include "enums/ComparisonFunc.hpp"
#include "enums/StencilOp.hpp"

namespace mce
{
	struct StencilFaceDescription
	{
		ComparisonFunc stencilFunc;
		StencilOp stencilDepthFailOp;
		StencilOp stencilPassOp;
		StencilOp stencilFailOp;
		
		StencilFaceDescription();

		bool operator==(const StencilFaceDescription& other) const;
		bool operator!=(const StencilFaceDescription& other) const;
	};
}
