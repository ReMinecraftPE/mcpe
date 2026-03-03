#include "RenderContextBase.hpp"

using namespace mce;

RenderContextBase::RenderContextBase()
{
    m_pRenderDevice = nullptr;
}

void RenderContextBase::loadMatrix(MatrixType matrixType, const Matrix& matrix)
{
}

void RenderContextBase::setVertexState(const VertexFormat& vertexFormat)
{
    m_lastVertexFormat = vertexFormat;
}

void RenderContextBase::clearVertexState(const VertexFormat& vertexFormat)
{
}

void RenderContextBase::enableFixedLighting(bool init)
{
}

void RenderContextBase::disableFixedLighting(bool teardown)
{
}

bool RenderContextBase::setShadeMode(ShadeMode mode)
{
    if (m_currentState.m_bBoundShadeMode && m_currentState.m_shadeMode == mode)
        return false;

    m_currentState.m_shadeMode = mode;

    return true;
}

bool RenderContextBase::setCurrentColor(const Color& color)
{
    if (m_currentState.m_bBoundColor && m_currentState.m_color == color)
        return false;

    m_currentState.m_color = color;

    return true;
}

bool RenderContextBase::setGamma(Gamma gamma)
{
    if (m_currentState.m_bBoundGamma && m_currentState.m_gamma == gamma)
        return false;

    m_currentState.m_gamma = gamma;

    return true;
}

void RenderContextBase::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
}

void RenderContextBase::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
}

void RenderContextBase::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
}

void RenderContextBase::setDepthRange(float nearVal, float farVal)
{
}

void RenderContextBase::setViewport(unsigned int width, unsigned int height, float nearVal, float farVal, const ViewportOrigin& origin)
{
}

void RenderContextBase::clearFrameBuffer(const Color& color)
{
}

void RenderContextBase::clearStencilBuffer()
{
}

void RenderContextBase::clearDepthStencilBuffer()
{
}

void RenderContextBase::clearContextState()
{
	m_currentState.clear();
    m_immediateBuffer = ImmediateBuffer();

    for (int i = 0; i < SHADER_TYPES_COUNT; i++)
    {
        m_lastShaderPrograms[i] = nullptr;
    }
}

void RenderContextBase::setRenderTarget()
{
}

void RenderContextBase::beginRender()
{
}

void RenderContextBase::endRender()
{
}

void RenderContextBase::suspend()
{
}

void RenderContextBase::resume()
{
}

void RenderContextBase::swapBuffers()
{
}

void RenderContextBase::lostContext()
{
    clearContextState();
}

RenderDevice* RenderContextBase::getDevice()
{
    return m_pRenderDevice;
}

void RenderContextBase::setStencilReference(uint8_t value)
{
    m_stencilReference = value;
}

uint8_t RenderContextBase::getStencilReference() const
{
    return m_stencilReference;
}

int RenderContextBase::getMaxVertexCount() const
{
    return -1;
}

bool RenderContextBase::supports8BitIndices() const
{
    return true;
}

bool RenderContextBase::supports32BitIndices() const
{
    return true;
}

bool RenderContextBase::supports16BitUnsignedUVs() const
{
    return true;
}
