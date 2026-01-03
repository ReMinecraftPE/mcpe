#include <stdexcept>
#include "RenderContextD3D11.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

RenderContextD3D11::InputLayoutID::InputLayoutID(const VertexFormat& vertexFormat, unsigned int attributeListIndex)
    : vertexFormat(vertexFormat)
    , attributeListIndex(attributeListIndex)
{
}

bool RenderContextD3D11::InputLayoutID::operator<(const RenderContextD3D11::InputLayoutID& other) const
{
    if (attributeListIndex != other.attributeListIndex)
        return attributeListIndex < other.attributeListIndex;

    return vertexFormat < other.vertexFormat;
}

bool RenderContextD3D11::InputLayoutID::operator==(const RenderContextD3D11::InputLayoutID& other) const
{
    return attributeListIndex == other.attributeListIndex
        && vertexFormat == other.vertexFormat;
}

RenderContextD3D11::RenderContextD3D11()
    : RenderContextBase()
{
    m_featureLevel = D3D_FEATURE_LEVEL_9_1;

    // wasn't here in 0.12.1, but where else is it supposed to go?
    createDeviceResources();
}

void RenderContextD3D11::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
    m_d3dContext->IASetPrimitiveTopology(modeMap[primitiveMode]);
    m_d3dContext->Draw(count, startOffset);
}

void RenderContextD3D11::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
    drawIndexed(primitiveMode, count, 0, indexSize);
}

void RenderContextD3D11::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
    m_d3dContext->IASetPrimitiveTopology(modeMap[primitiveMode]);
    m_d3dContext->DrawIndexed(count, startOffset, 0);
}

void RenderContextD3D11::setDepthRange(float nearVal, float farVal)
{
    m_viewport.MinDepth = nearVal;
    m_viewport.MaxDepth = farVal;
    m_d3dContext->RSSetViewports(1, &m_viewport);
}

void RenderContextD3D11::setViewport(unsigned int width, unsigned int height, float nearVal, float farVal, const ViewportOrigin& origin)
{
    m_viewport.TopLeftX = origin.leftX;
    m_viewport.TopLeftY = origin.topLeftY;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinDepth = nearVal;
    m_viewport.MaxDepth = farVal;
    m_d3dContext->RSSetViewports(1, &m_viewport);
}

void RenderContextD3D11::clearFrameBuffer(const Color& color)
{
    m_d3dContext->ClearRenderTargetView(**m_renderTargetView, (const FLOAT*)&color);
}

void RenderContextD3D11::clearStencilBuffer()
{
    // WP 0.12.1 originally called clearStencilBuffer() here
    m_d3dContext->ClearDepthStencilView(**m_depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderContextD3D11::clearDepthStencilBuffer()
{
    m_d3dContext->ClearDepthStencilView(**m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderContextD3D11::clearContextState()
{
    RenderContextBase::clearContextState();
}

void RenderContextD3D11::setRenderTarget()
{
    m_d3dContext->OMSetRenderTargets(1, *m_renderTargetView, **m_depthStencilView);
}

void RenderContextD3D11::swapBuffers()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0x0);

    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    m_d3dContext->DiscardView(**m_renderTargetView);

    // Discard the contents of the depth stencil.
    m_d3dContext->DiscardView(**m_depthStencilView);

    if (hr != DXGI_ERROR_DEVICE_REMOVED && hr != DXGI_ERROR_DEVICE_RESET)
    {
        ErrorHandlerDXGI::checkForErrors(hr);
    }
}

void RenderContextD3D11::createDeviceResources()
{
    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    ComInterface<ID3D11Device> device;
    ComInterface<ID3D11DeviceContext> context;

    // Create ID3D11Device
    {
        UINT creationFlags = 0x0;

#ifdef _DEBUG
        // If the project is in a debug build, enable the debug layer.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hResult = D3D11CreateDevice(
        /* pAdapter           */ NULL,                     // Specify nullptr to use the default adapter.
        /* DriverType         */ D3D_DRIVER_TYPE_HARDWARE, // Create a device using the hardware graphics driver.
        /* Software           */ 0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        /* Flags              */ creationFlags,            // Set debug and Direct2D compatibility flags.
        /* pFeatureLevels     */ featureLevels,            // List of feature levels this app can support.
        /* FeatureLevels      */ ARRAYSIZE(featureLevels), // Size of the list above.
        /* SDKVersion         */ D3D11_SDK_VERSION,        // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        /* ppDevice           */ *device,                  // Returns the Direct3D device created.
        /* pFeatureLevel      */ &m_featureLevel,          // Returns feature level of device created.
        /* ppImmediateContext */ *context                  // Returns the device immediate context.
        );
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Store pointers to the Direct3D 11.1 API device and immediate context.

    // Get ID3D11Device2
    {
        m_d3dDevice.release();
        HRESULT hResult = device->QueryInterface(IID_ID3D11Device2, (void**)*m_d3dDevice);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Get ID3D11DeviceContext2
    {
        m_d3dContext.release();
        HRESULT hResult = context->QueryInterface(IID_ID3D11DeviceContext2, (void**)*m_d3dContext);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Get ID3DUserDefinedAnnotation
    {
        m_userDefinedAnnotation.release();
        HRESULT hResult = context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)*m_userDefinedAnnotation);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
}

// Mojang likely based this on:
// https://github.com/microsoft/Windows-universal-samples/blob/082195895276903b6630d5cb4d03c9d365ec210c/Samples/LowLatencyInput/cpp/Common/DeviceResources.cpp#L218
// These resources need to be recreated every time the window size is changed.
void RenderContextD3D11::createWindowSizeDependentResources(HWND hWnd, const Vec2& logicalSize, const Vec2& compositionScale)
{
    m_logicalSize = logicalSize;
    m_compositionScale = compositionScale;

    // Calculate the necessary swap chain and render target size in pixels.
    Vec2 outputSize = m_logicalSize * m_compositionScale;
    //std::swap(outputSize.x, outputSize.y); // D3D needs this for whatever reason

    // Prevent zero size Direct3D content from being created.
    outputSize.x = Mth::Max(outputSize.x, 1.0f);
    outputSize.y = Mth::Max(outputSize.y, 1.0f);

    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

    m_renderTargetView.release();
    m_depthStencilView.release();
    m_d3dContext->Flush();

    if (m_swapChain)
    {
        // If the swap chain already exists, resize it.
        if (!resizeSwapChain(outputSize))
        {
            m_swapChain.release();

            // Recreate device and try again
            createDeviceResources();
            createWindowSizeDependentResources(hWnd, m_logicalSize, m_compositionScale);
            return;
        }
    }
    else
    {
        // Otherwise, create a new one using the same adapter as the existing Direct3D device.
        initSwapChain(hWnd, outputSize);
    }

    ComInterface<IDXGISwapChain2> swapChain2;
    {
        HRESULT hResult = m_swapChain->QueryInterface(IID_IDXGISwapChain2, (void**)*swapChain2);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    if (!hWnd)
    {
        // Setup inverse scale on the swap chain
        DXGI_MATRIX_3X2_F inverseScale = { 0 };
        inverseScale._11 = 1.0f / m_compositionScale.x;
        inverseScale._22 = 1.0f / m_compositionScale.y;

        HRESULT hResult = swapChain2->SetMatrixTransform(&inverseScale);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Create a render target view of the swap chain back buffer.
    {
        ComInterface<ID3D11Texture2D> backBuffer;

        HRESULT hResult = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)*backBuffer);
        ErrorHandlerDXGI::checkForErrors(hResult);

        m_renderTargetView.release();
        hResult = m_d3dDevice->CreateRenderTargetView(**backBuffer, NULL, *m_renderTargetView);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    ComInterface<ID3D11Texture2D> depthStencil;
    {
        // Create a depth stencil view for use with 3D rendering if needed.
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            lroundf(outputSize.x),
            lroundf(outputSize.y),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
        );

        HRESULT hResult = m_d3dDevice->CreateTexture2D(&depthStencilDesc, NULL, *depthStencil);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Create DepthStencilView
    {
        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

        m_depthStencilView.release();
        HRESULT hResult = m_d3dDevice->CreateDepthStencilView(**depthStencil, &depthStencilViewDesc, *m_depthStencilView);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    setViewport(outputSize.x, outputSize.y, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH, ViewportOrigin::ZERO);
    setRenderTarget();
}

void RenderContextD3D11::initSwapChain(HWND hWnd, const Vec2& size)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
    {
        swapChainDesc.Width = lroundf(size.x);
        swapChainDesc.Height = lroundf(size.y);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
        swapChainDesc.Flags = 0x0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    }

    ComInterface<IDXGIDevice3> dxgiDevice;
    {
        HRESULT hResult = m_d3dDevice->QueryInterface(IID_IDXGIDevice3, (void**)*dxgiDevice);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    ComInterface<IDXGIAdapter> dxgiAdapter;
    {
        HRESULT hResult = dxgiDevice->GetAdapter(*dxgiAdapter);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    ComInterface<IDXGIFactory2> dxgiFactory;
    {
        HRESULT hResult = dxgiAdapter->GetParent(IID_IDXGIFactory2, (void**)*dxgiFactory);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Create IDXGISwapChain1
    {
        m_swapChain.release();
        HRESULT hResult;
        if (hWnd)
        {
            // @TODO: DXGI_SWAP_CHAIN_FULLSCREEN_DESC
            hResult = dxgiFactory->CreateSwapChainForHwnd(**m_d3dDevice, hWnd, &swapChainDesc, NULL, NULL, *m_swapChain);
        }
        else
        {
            hResult = dxgiFactory->CreateSwapChainForComposition(**m_d3dDevice, &swapChainDesc, NULL, *m_swapChain);
        }
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
}

bool RenderContextD3D11::resizeSwapChain(const Vec2& size)
{
    HRESULT hResult = m_swapChain->ResizeBuffers(
        2, // Single-buffered swap chain.
        lroundf(size.x),
        lroundf(size.y),
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0x0);
    if (hResult == DXGI_ERROR_DEVICE_REMOVED || hResult == DXGI_ERROR_DEVICE_RESET)
    {
        // If the device was removed for any reason, a new device and swap chain will need to be created.
        return false;
    }
    ErrorHandlerDXGI::checkForErrors(hResult);

    return true;
}

D3DDevice RenderContextD3D11::getD3DDevice()
{
    return m_d3dDevice;
}

D3DDeviceContext RenderContextD3D11::getD3DDeviceContext()
{
    return m_d3dContext;
}

bool RenderContextD3D11::supportsR8G8B8A8_SNORM() const
{
    return m_featureLevel >= D3D_FEATURE_LEVEL_10_0;
}

bool RenderContextD3D11::supportsR16G16_UNORM() const
{
    return m_featureLevel >= D3D_FEATURE_LEVEL_10_0;
}

D3D11_COMPARISON_FUNC mce::getComparisonFunc(ComparisonFunc comparisonFunc)
{
    switch (comparisonFunc)
    {
    case COMPARISON_FUNC_EQUAL:         return D3D11_COMPARISON_EQUAL;
    case COMPARISON_FUNC_NOT_EQUAL:     return D3D11_COMPARISON_NOT_EQUAL;
    case COMPARISON_FUNC_ALWAYS:        return D3D11_COMPARISON_ALWAYS;
    case COMPARISON_FUNC_LESS:          return D3D11_COMPARISON_LESS;
    case COMPARISON_FUNC_GREATER:       return D3D11_COMPARISON_GREATER;
    case COMPARISON_FUNC_GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
    case COMPARISON_FUNC_LESS_EQUAL:    return D3D11_COMPARISON_LESS_EQUAL;
    default:
        LOG_E("Unknown comparisonFunc: %d", comparisonFunc);
        throw std::bad_cast();
    }
}