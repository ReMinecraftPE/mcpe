#pragma once

#include "enums/ComparisonFunc.hpp"

namespace mce
{
	struct FixedPipelineStateDescription
	{
		bool enableAlphaTest;
		ComparisonFunc alphaFunc;
		float alphaRef;
		bool enableTexture;

		FixedPipelineStateDescription();

		bool operator==(const FixedPipelineStateDescription& other) const;
		bool operator!=(const FixedPipelineStateDescription& other) const;
	};
}
