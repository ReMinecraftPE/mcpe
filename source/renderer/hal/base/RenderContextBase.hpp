#pragma once

#include "RenderContextStateBase.hpp"
#include "common/math/Color.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/MatrixStack.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "renderer/hal/enums/ShadeMode.hpp"
#include "renderer/hal/enums/ShaderType.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"
#include "renderer/hal/interface/ImmediateBuffer.hpp"
#include "renderer/hal/interface/RenderDevice.hpp"

namespace mce
{
    class RenderContextBase
    {
    public:
        RenderContextStateBase m_currentState;
        VertexFormat m_lastVertexFormat;
        Color m_currentColor;
        ShadeMode m_currentShadeMode;
        // Unused in OGL
        int field_34;
        // Only used in DX11
        ShaderProgram* m_lastShaderPrograms[SHADER_TYPES_COUNT];
        ImmediateBuffer m_immediateBuffer;
        StencilRefObject m_stencilReference;
        RenderDevice *m_pRenderDevice;

    public:
        RenderContextBase();

    public:
        void loadMatrix(MatrixType matrixType, const Matrix& matrix);
        void setVertexState(const VertexFormat& vertexFormat);
        void clearVertexState(const VertexFormat& vertexFormat);
        void enableFixedLighting(bool init);
        void disableFixedLighting(bool teardown);
        bool setShadeMode(ShadeMode mode);
        bool setCurrentColor(const Color& color);
        void draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize);
        void setDepthRange(float nearVal, float farVal);
        void setViewport(int topLeftX, int topLeftY, unsigned int width, unsigned int height, float nearVal, float farVal);
        void clearFrameBuffer(const Color& color);
        void clearStencilBuffer();
        void clearDepthStencilBuffer();
        void clearContextState();
        void setRenderTarget();
        void swapBuffers();
        void lostContext();

        RenderDevice* getDevice();
        void setStencilReference(uint8_t value);
        uint8_t getStencilReference() const;

        int getMaxVertexCount() const;
        bool supports32BitIndices() const;
        bool supports16BitUnsignedUVs() const;
    };
}