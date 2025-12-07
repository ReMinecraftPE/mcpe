#include "DepthStencilStateDescription.hpp"

using namespace mce;

DepthStencilStateDescription::DepthStencilStateDescription()
    : stencilRef(0)
{
    depthFunc = COMPARISON_FUNC_LESS;
    depthTestEnabled = true;
    stencilTestEnabled = false;
    frontFace = StencilFaceDescription();
    backFace = StencilFaceDescription();
    depthWriteMask = DEPTH_WRITE_MASK_ALL;

    overwroteStencilRef = false;
    stencilReadMask = 0xFFFFFFFF;
    stencilWriteMask = 0xFFFFFFFF;
}

bool DepthStencilStateDescription::operator==(const DepthStencilStateDescription& other) const
{
	return depthTestEnabled == other.depthTestEnabled &&
           frontFace == other.frontFace &&
		   backFace == other.backFace &&
		   depthWriteMask == other.depthWriteMask &&
		   stencilReadMask == other.stencilReadMask &&
           stencilWriteMask == other.stencilWriteMask &&
           stencilRef == other.stencilRef &&
           overwroteStencilRef == other.overwroteStencilRef;
}

bool DepthStencilStateDescription::operator!=(const DepthStencilStateDescription& other) const
{
	return !(*this == other);
}