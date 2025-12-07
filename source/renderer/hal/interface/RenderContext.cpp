#include "RenderContext.hpp"

using namespace mce;

RenderContext::RenderContext()
	: MCE_GFX_CLASS(RenderContext)()
{
}

void RenderContext::lostContext()
{
	clearContextState();
	MCE_GFX_CLASS(RenderContext)::lostContext();
}