#include "StencilFaceDescription.hpp"

using namespace mce;

StencilFaceDescription::StencilFaceDescription()
{
	stencilFunc = COMPARISON_FUNC_ALWAYS;
	stencilDepthFailOp = STENCIL_OP_KEEP;
	stencilPassOp = STENCIL_OP_KEEP;
	stencilFailOp = STENCIL_OP_KEEP;
}

bool StencilFaceDescription::operator==(const StencilFaceDescription& other) const
{
	return this->stencilFunc == other.stencilFunc &&
		   this->stencilDepthFailOp == other.stencilDepthFailOp &&
		   this->stencilPassOp == other.stencilPassOp &&
		   this->stencilFailOp == other.stencilFailOp;
}

bool StencilFaceDescription::operator!=(const StencilFaceDescription& other) const
{
	return !(*this == other);
}
