#pragma once

#include "common/math/Color.hpp"
#include "enums/FogMode.hpp"

namespace mce
{
	struct FogStateDescription
	{
		bool enableFog;
		FogMode fogMode;
		float fogDensity;
		float fogStartZ;
		float fogEndZ;
		Color fogColor;

		FogStateDescription();

		bool operator==(const FogStateDescription& other) const;
		bool operator!=(const FogStateDescription& other) const;
	};
}
