#pragma once

#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER(DepthStencilState)

namespace mce
{
    class DepthStencilState : public MCE_GFX_CLASS(DepthStencilState)
    {
    public:
        DepthStencilState();
    };
}