#pragma once

#include "enums/ComparisonFunc.hpp"

namespace mce
{
	struct AlphaStateDescription
	{
		bool enableAlphaTest;
		ComparisonFunc alphaFunc;
		float alphaRef;

		AlphaStateDescription();

		bool operator==(const AlphaStateDescription& other) const;
		bool operator!=(const AlphaStateDescription& other) const;
	};
}
