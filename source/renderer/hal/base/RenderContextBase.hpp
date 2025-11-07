#pragma once

#include "RenderContextStateBase.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/interface/ImmediateBuffer.hpp"
#include "renderer/hal/interface/RenderDevice.hpp"

namespace mce
{
    class RenderContextBase
    {
    public:
        RenderContextStateBase m_currentState;
        VertexFormat m_lastVertexFormat;
        // These are unused in OGL HAL
        int field_34;
        int field_38;
        int field_3C;
        int field_40;
        ImmediateBuffer m_immediateBuffer;
        StencilRefObject m_stencilReference;
        RenderDevice *m_renderDevice;

    public:
        RenderContextBase();

    public:
        RenderDevice* getDevice();
        void lostContext();
        void setStencilReference(unsigned int value);
        unsigned int getStencilReference() const;
    };
}