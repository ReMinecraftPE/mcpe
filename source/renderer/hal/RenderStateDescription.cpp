#include "RenderStateDescription.hpp"

using namespace mce;

RenderStateDescription::RenderStateDescription()
{
	enableTexture = false;
}

bool RenderStateDescription::operator==(const RenderStateDescription& other) const
{
	return enableTexture == other.enableTexture;
}

bool RenderStateDescription::operator!=(const RenderStateDescription& other) const
{
	return !(*this == other);
}
