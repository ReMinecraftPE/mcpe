#include "BlendStateDescription.hpp"

using namespace mce;

BlendStateDescription::BlendStateDescription()
{
	blendSource = BLEND_TARGET_SOURCE_ALPHA;
	blendDestination = BLEND_TARGET_ONE_MINUS_SRC_ALPHA;
	colorWriteMask = COLOR_WRITE_MASK_ALL;
	enableBlend = false;
}

bool BlendStateDescription::operator==(const BlendStateDescription& other) const
{
	return blendSource == other.blendSource &&
		   blendDestination == other.blendDestination &&
		   colorWriteMask == other.colorWriteMask &&
		   enableBlend == other.enableBlend;
}

bool BlendStateDescription::operator!=(const BlendStateDescription& other) const
{
	return !(*this == other);
}
