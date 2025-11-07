#pragma once

#include "common/utility/Singleton.hpp"
#include "renderer/hal/ogl/RenderDeviceOGL.hpp"

namespace mce
{
    class RenderDevice : public RenderDeviceOGL, public Singleton<RenderDevice>
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