#include "FixedPipelineStateDescription.hpp"

using namespace mce;

FixedPipelineStateDescription::FixedPipelineStateDescription()
{
	enableAlphaTest = false;
	alphaFunc = COMPARISON_FUNC_GREATER_EQUAL;
	alphaRef = 0.5f;
	/* pre-HAL values
	alphaFunc = COMPARISON_FUNC_GREATER;
	alphaRef = 0.1f;*/
	enableTexture = false;
}

bool FixedPipelineStateDescription::operator==(const FixedPipelineStateDescription& other) const
{
	return enableAlphaTest == other.enableAlphaTest &&
		   alphaFunc == other.alphaFunc &&
		   alphaRef == other.alphaRef &&
		   enableTexture == other.enableTexture;
}

bool FixedPipelineStateDescription::operator!=(const FixedPipelineStateDescription& other) const
{
	return !(*this == other);
}
