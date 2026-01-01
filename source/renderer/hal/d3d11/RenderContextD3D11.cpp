#include <stdexcept>
#include "RenderContextD3D11.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

RenderContextD3D11::RenderContextD3D11()
    : RenderContextBase()
{
    m_featureLevel = D3D_FEATURE_LEVEL_9_1;

    // wasn't here in 0.12.1, but where else is it supposed to go?
    createD3DDevice();
}

void RenderContextD3D11::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
    m_D3DDeviceContext->IASetPrimitiveTopology(modeMap[primitiveMode]);
    m_D3DDeviceContext->Draw(count, startOffset);
}

void RenderContextD3D11::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
    drawIndexed(primitiveMode, count, 0, indexSize);
}

void RenderContextD3D11::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
    m_D3DDeviceContext->IASetPrimitiveTopology(modeMap[primitiveMode]);
    m_D3DDeviceContext->DrawIndexed(count, startOffset, indexSize);
}

void RenderContextD3D11::setDepthRange(float nearVal, float farVal)
{
    m_viewport.MinDepth = nearVal;
    m_viewport.MaxDepth = farVal;
    m_D3DDeviceContext->RSSetViewports(1, &m_viewport);
}

void RenderContextD3D11::setViewport(int topLeftX, int topLeftY, unsigned int width, unsigned int height, float nearVal, float farVal)
{
    m_viewport.TopLeftX = topLeftX;
    m_viewport.TopLeftY = topLeftY;
    m_viewport.Width = height;
    m_viewport.Height = width;
    m_viewport.MinDepth = nearVal;
    m_viewport.MaxDepth = farVal;
    m_D3DDeviceContext->RSSetViewports(1, &m_viewport);
}

void RenderContextD3D11::clearFrameBuffer(const Color& color)
{
    m_D3DDeviceContext->ClearRenderTargetView(**m_renderTargetView, (const FLOAT*)&color);
}

void RenderContextD3D11::clearStencilBuffer()
{
    clearDepthStencilBuffer();
}

void RenderContextD3D11::clearDepthStencilBuffer()
{
    m_D3DDeviceContext->ClearDepthStencilView(**m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}

void RenderContextD3D11::clearContextState()
{
    RenderContextBase::clearContextState();
}

void RenderContextD3D11::setRenderTarget()
{
    m_D3DDeviceContext->OMSetRenderTargets(1, *m_renderTargetView, **m_depthStencilView);
}

void RenderContextD3D11::swapBuffers()
{
    HRESULT hResult = m_swapChain->Present(1, 0x0);
    m_D3DDeviceContext->DiscardView(**m_renderTargetView);
    m_D3DDeviceContext->DiscardView(**m_depthStencilView);

    if (hResult != DXGI_ERROR_DEVICE_REMOVED && hResult != DXGI_ERROR_DEVICE_RESET)
    {
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
}

void RenderContextD3D11::createD3DDevice()
{
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    ComInterface<ID3D11Device> device;
    ComInterface<ID3D11DeviceContext> deviceContext;

    // Create ID3D11Device
    {
        unsigned int flags = 0x0;

#ifdef _DEBUG
        // If the project is in a debug build, enable the debug layer.
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hResult = D3D11CreateDevice(
        /* pAdapter           */ NULL,
        /* DriverType         */ D3D_DRIVER_TYPE_HARDWARE,
        /* Software           */ NULL,
        /* Flags              */ flags,
        /* pFeatureLevels     */ featureLevels,
        /* FeatureLevels      */ sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
        /* SDKVersion         */ 7,
        /* ppDevice           */ *device,
        /* pFeatureLevel      */ &m_featureLevel,
        /* ppImmediateContext */ *deviceContext);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Get ID3D11Device2
    {
        m_D3DDevice.release();
        HRESULT hResult = device->QueryInterface(IID_ID3D11Device2, (void**)*m_D3DDevice);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Get ID3D11DeviceContext2
    {
        m_D3DDeviceContext.release();
        HRESULT hResult = deviceContext->QueryInterface(IID_ID3D11DeviceContext2, (void**)*m_D3DDeviceContext);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Get ID3DUserDefinedAnnotation
    {
        m_userDefinedAnnotation.release();
        HRESULT hResult = deviceContext->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)*m_userDefinedAnnotation);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
}

// Mojang likely based this on:
// https://github.com/microsoft/Windows-universal-samples/blob/082195895276903b6630d5cb4d03c9d365ec210c/Samples/LowLatencyInput/cpp/Common/DeviceResources.cpp#L218
void RenderContextD3D11::initContext(const Vec2& logicalSize, const Vec2& compositionScale)
{
    m_logicalSize = logicalSize;
    m_compositionScale = compositionScale;

    // Calculate the necessary swap chain and render target size in pixels.
    Vec2 outputSize = m_logicalSize * m_compositionScale;

    // Prevent zero size Direct3D content from being created.
    outputSize.x = Mth::Max(outputSize.x, 1.0f);
    outputSize.y = Mth::Max(outputSize.y, 1.0f);

    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_D3DDeviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

    m_renderTargetView.release();
    m_depthStencilView.release();
    m_D3DDeviceContext->Flush();

    if (m_swapChain)
    {
        // If the swap chain already exists, resize it.
        if (!resizeSwapChain(outputSize));
        {
            m_swapChain.release();

            // Recreate device and try again
            createD3DDevice();
            initContext(m_logicalSize, m_compositionScale);
            return;
        }
    }
    else
    {
        initSwapChain(outputSize);
    }

    ComInterface<IDXGISwapChain2> swapChain2;
    {
        HRESULT hResult = m_swapChain->QueryInterface(IID_IDXGISwapChain2, (void**)*swapChain2);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    {
        // Setup inverse scale on the swap chain
        DXGI_MATRIX_3X2_F inverseScale = { 0 };
        inverseScale._11 = 1.0f / outputSize.x;
        inverseScale._22 = 1.0f / outputSize.y;

        HRESULT hResult = swapChain2->SetMatrixTransform(&inverseScale);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    ComInterface<ID3D11Resource> resource;
    {
        HRESULT hResult = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)*resource);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Create RenderTargetView
    {
        m_renderTargetView.release();
        HRESULT hResult = m_D3DDevice->CreateRenderTargetView(**resource, NULL, *m_renderTargetView);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    ComInterface<ID3D11Texture2D> texture;
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        {
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            textureDesc.CPUAccessFlags = 0x0;
            textureDesc.MiscFlags = 0x0;
            textureDesc.Width = lroundf(outputSize.x);
            textureDesc.Height = lroundf(outputSize.y);
        }

        HRESULT hResult = m_D3DDevice->CreateTexture2D(&textureDesc, NULL, *texture);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    // Create DepthStencilView
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        {
            depthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthStencilViewDesc.Flags = 0;
            depthStencilViewDesc.Texture1D.MipSlice = 0;
        }

        m_depthStencilView.release();
        HRESULT hResult = m_D3DDevice->CreateDepthStencilView(**texture, &depthStencilViewDesc, *m_depthStencilView);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    setViewport(0.0f, 0.0f, outputSize.x, outputSize.y, 0.0f, 1.0f);
    setRenderTarget();
}

void RenderContextD3D11::initSwapChain(const Vec2& size)
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
        HRESULT hResult = m_D3DDevice->QueryInterface(IID_IDXGIDevice3, (void**)*dxgiDevice);
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
        HRESULT hResult = dxgiFactory->CreateSwapChainForComposition(**m_D3DDevice, &swapChainDesc, NULL, *m_swapChain);
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
    return m_D3DDevice;
}

D3DDeviceContext RenderContextD3D11::getD3DDeviceContext()
{
    return m_D3DDeviceContext;
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