#include "renderer/hal/interface/RenderDevice.hpp"
#include "RenderContextImmediate.hpp"

using namespace mce;

RenderContext& RenderContextImmediate::get()
{
    return RenderDevice::getInstance().getRenderContext();
}

const RenderContext& RenderContextImmediate::getAsConst()
{
    return RenderDevice::getInstance().getRenderContextAsConst();
}