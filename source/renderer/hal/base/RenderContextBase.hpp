#pragma once

#include "RenderContextStateBase.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
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
        void draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize);
        void lostContext();

        RenderDevice* getDevice();
        void setStencilReference(unsigned int value);
        unsigned int getStencilReference() const;

        static int getMaxVertexCount();
    };
}