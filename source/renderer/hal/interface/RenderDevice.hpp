#pragma once

#include "common/utility/Singleton.hpp"
#include "renderer/PlatformDefinitions.h"
#include MCE_GFX_CLASS_HEADER(RenderDevice)

namespace mce
{
    class RenderDevice : public MCE_GFX_CLASS(RenderDevice), public Singleton<RenderDevice>
    {
    public:
        RenderContext *m_immediateContext;

    public:
        RenderDevice();
        ~RenderDevice();

    public:
        RenderContext& getRenderContext();
        const RenderContext& getRenderContextAsConst() const;
    };
}
