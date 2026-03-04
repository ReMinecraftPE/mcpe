#include "AlphaStateDescription.hpp"

using namespace mce;

AlphaStateDescription::AlphaStateDescription()
{
	enableAlphaTest = false;
	alphaFunc = COMPARISON_FUNC_GREATER_EQUAL;
	alphaRef = 0.5f;
	// pre-HAL values
	/*alphaFunc = COMPARISON_FUNC_GREATER;
	alphaRef = 0.1f;*/
}

bool AlphaStateDescription::operator==(const AlphaStateDescription& other) const
{
	return enableAlphaTest == other.enableAlphaTest &&
		alphaFunc == other.alphaFunc &&
		alphaRef == other.alphaRef;
}

bool AlphaStateDescription::operator!=(const AlphaStateDescription& other) const
{
	return !(*this == other);
}
