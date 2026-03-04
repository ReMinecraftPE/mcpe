#include "RenderContext.hpp"

using namespace mce;

void RenderContext::setViewport(unsigned int width, unsigned int height, float nearVal, float farVal, const ViewportOrigin& origin)
{
	MCE_GFX_CLASS(RenderContext)::setViewport(width, height, nearVal, farVal, origin);
}

void RenderContext::lostContext()
{
	clearContextState();
	MCE_GFX_CLASS(RenderContext)::lostContext();
}
