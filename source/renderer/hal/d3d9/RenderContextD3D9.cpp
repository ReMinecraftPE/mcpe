#include <stdexcept>
#include "RenderContextD3D9.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

unsigned int _getPrimitiveCount(PrimitiveMode mode, unsigned int indexCount)
{
    // D3D11 uses index count, but D3D9 needs the actual number of primitives.
    switch (mode)
    {
    case PRIMITIVE_MODE_QUAD_LIST:
    case PRIMITIVE_MODE_TRIANGLE_LIST:  return indexCount / 3;
    case PRIMITIVE_MODE_TRIANGLE_STRIP: return indexCount - 2;
    case PRIMITIVE_MODE_LINE_LIST:      return indexCount / 2;
    case PRIMITIVE_MODE_LINE_STRIP:     return indexCount - 1;
    default:
        LOG_E("Unknown PrimitiveMode: %d", mode);
        assert(false); // we don't want this shitting itself on release
        return 0;
    }
}

RenderContextD3D9::VertexDeclID::VertexDeclID(const VertexFormat& vertexFormat, unsigned int attributeListIndex)
    : vertexFormat(vertexFormat)
    , attributeListIndex(attributeListIndex)
{
}

bool RenderContextD3D9::VertexDeclID::operator<(const RenderContextD3D9::VertexDeclID& other) const
{
    if (attributeListIndex != other.attributeListIndex)
        return attributeListIndex < other.attributeListIndex;

    return vertexFormat < other.vertexFormat;
}

bool RenderContextD3D9::VertexDeclID::operator==(const RenderContextD3D9::VertexDeclID& other) const
{
    return attributeListIndex == other.attributeListIndex
        && vertexFormat == other.vertexFormat;
}

RenderContextD3D9::RenderContextD3D9()
    : RenderContextBase()
{
    memset(&m_viewport, 0, sizeof(m_viewport));

    // wasn't here in 0.12.1, but where else is it supposed to go?
    createDeviceResources();
}

void RenderContextD3D9::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
    ErrorHandlerD3D9::checkForErrors(
        m_d3dDevice->DrawPrimitive(modeMap[primitiveMode], startOffset, _getPrimitiveCount(primitiveMode, count))
    );
}

void RenderContextD3D9::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
    drawIndexed(primitiveMode, count, 0, indexSize);
}

void RenderContextD3D9::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
    ErrorHandlerD3D9::checkForErrors(
        m_d3dDevice->DrawIndexedPrimitive(modeMap[primitiveMode], 0, 0, 0, startOffset, _getPrimitiveCount(primitiveMode, count))
    );
}

void RenderContextD3D9::setDepthRange(float nearVal, float farVal)
{
    m_viewport.MinZ = nearVal;
    m_viewport.MaxZ = farVal;
    m_d3dDevice->SetViewport(&m_viewport);
}

void RenderContextD3D9::setViewport(unsigned int width, unsigned int height, float nearVal, float farVal, const ViewportOrigin& origin)
{
    m_viewport.X = origin.leftX;
    m_viewport.Y = origin.topLeftY;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinZ = nearVal;
    m_viewport.MaxZ = farVal;
    m_d3dDevice->SetViewport(&m_viewport);
}

void RenderContextD3D9::clearFrameBuffer(const Color& color)
{
    m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a), 1.0f, 0);
}

void RenderContextD3D9::clearStencilBuffer()
{
    const Color& c = Color::WHITE;
    m_d3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a), 1.0f, 0);
}

void RenderContextD3D9::clearDepthStencilBuffer()
{
    const Color& c = Color::WHITE;
    m_d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a), 1.0f, 0);
}

void RenderContextD3D9::clearContextState()
{
    RenderContextBase::clearContextState();
}

void RenderContextD3D9::beginRender()
{
    m_d3dDevice->BeginScene();
}

void RenderContextD3D9::endRender()
{
    m_d3dDevice->EndScene();
}

void RenderContextD3D9::swapBuffers()
{
    HRESULT hr = m_d3dDevice->Present(NULL, NULL, NULL, NULL);
    ErrorHandlerD3D9::checkForErrors(hr);
}

bool RenderContextD3D9::supports8BitIndices() const
{
    // Direct3D 9 does not natively support 8-bit index buffers.
    // Most hardware requires at least 16-bit.
    return false;
}

void RenderContextD3D9::createDeviceResources()
{
    // Create the Direct3D 9 API and device objects.
    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!m_pD3D)
    {
        LOG_E("Failed to initialize Direct3D9!");
        throw std::bad_cast();
    }
}

// These resources need to be recreated every time the window size is changed.
void RenderContextD3D9::createWindowSizeDependentResources(HWND hWnd, unsigned int width, unsigned int height)
{
    m_d3dDevice.release();

    // Create IDirect3DDevice9
    {
        D3DPRESENT_PARAMETERS d3dpp = { 0 };
        {
            d3dpp.BackBufferWidth = width;
            d3dpp.BackBufferHeight = height;
            d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
            d3dpp.BackBufferCount = 1;
            d3dpp.EnableAutoDepthStencil = TRUE;
            d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
            d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            if (hWnd)
            {
                d3dpp.hDeviceWindow = hWnd;
                d3dpp.Windowed = TRUE;
            }
        }

        DWORD flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        HRESULT hResult = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, flags, &d3dpp, *m_d3dDevice);
        ErrorHandlerD3D9::checkForErrors(hResult);

        m_d3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
        m_d3dDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
        //m_d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    }

    setViewport(width, height, 0.0f, 1.0f, ViewportOrigin::ZERO);
}

mce::D3DDevice RenderContextD3D9::getD3DDevice()
{
    return m_d3dDevice;
}

D3DCMPFUNC mce::getComparisonFunc(ComparisonFunc comparisonFunc)
{
    switch (comparisonFunc)
    {
    case COMPARISON_FUNC_EQUAL:         return D3DCMP_EQUAL;
    case COMPARISON_FUNC_NOT_EQUAL:     return D3DCMP_NOTEQUAL;
    case COMPARISON_FUNC_ALWAYS:        return D3DCMP_ALWAYS;
    case COMPARISON_FUNC_LESS:          return D3DCMP_LESS;
    case COMPARISON_FUNC_GREATER:       return D3DCMP_GREATER;
    case COMPARISON_FUNC_GREATER_EQUAL: return D3DCMP_GREATEREQUAL;
    case COMPARISON_FUNC_LESS_EQUAL:    return D3DCMP_LESSEQUAL;
    default:
        LOG_E("Unknown comparisonFunc: %d", comparisonFunc);
        throw std::bad_cast();
    }
}