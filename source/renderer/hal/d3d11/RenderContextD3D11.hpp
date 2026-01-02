#pragma once

#include <vector>
#include <map>

#include "API_D3D11.hpp"
#include "renderer/hal/base/RenderContextBase.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"
#include "world/phys/Vec2.hpp"

namespace mce
{
    const D3D_PRIMITIVE_TOPOLOGY modeMap[] = {
        /*PRIMITIVE_MODE_NONE*/           D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
        /*PRIMITIVE_MODE_QUAD_LIST*/      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        /*PRIMITIVE_MODE_TRIANGLE_LIST*/  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        /*PRIMITIVE_MODE_TRIANGLE_STRIP*/ D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
        /*PRIMITIVE_MODE_LINE_LIST*/      D3D_PRIMITIVE_TOPOLOGY_LINELIST,
        /*PRIMITIVE_MODE_LINE_STRIP*/     D3D_PRIMITIVE_TOPOLOGY_LINESTRIP
    };

    typedef ComInterface<ID3D11Device2> D3DDevice;
    typedef ComInterface<ID3D11DeviceContext2> D3DDeviceContext;

    class DepthStencilState;
    class RenderContextD3D11 : public RenderContextBase
    {
    public:
        struct InputLayoutID
        {
            VertexFormat vertexFormat;
            unsigned int attributeListIndex;

            /*InputLayoutID()
                : vertexFormat(VertexFormat::EMPTY)
                , attributeListIndex(0)
            {
            }*/

            InputLayoutID(const VertexFormat& vertexFormat, unsigned int attributeListIndex)
                : vertexFormat(vertexFormat)
                , attributeListIndex(attributeListIndex)
            {
            }

            bool operator<(const InputLayoutID& other) const
            {
                return attributeListIndex < other.attributeListIndex;
            }
        };
        typedef std::map<InputLayoutID, ComInterface<ID3D11InputLayout>> InputLayoutCache;

    private:
        D3DDevice m_d3dDevice;
        D3DDeviceContext m_d3dContext;
        ComInterface<IDXGISwapChain1> m_swapChain;

    public:
        ComInterface<ID3D11RenderTargetView> m_renderTargetView;
        ComInterface<ID3D11DepthStencilView> m_depthStencilView;
        CD3D11_VIEWPORT m_viewport;
        ComInterface<ID3DUserDefinedAnnotation> m_userDefinedAnnotation;
        D3D_FEATURE_LEVEL m_featureLevel;
        Vec2 m_logicalSize;
        Vec2 m_compositionScale;
        InputLayoutCache m_inputLayoutCache;
        ID3D11Buffer* m_constantBuffers[SHADER_TYPES_COUNT][60];

    public:
        RenderContextD3D11();

    public:
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

        void createDeviceResources();
        void createWindowSizeDependentResources(HWND hWnd, const Vec2& logicalSize, const Vec2& compositionScale);
        void initSwapChain(HWND hWnd, const Vec2& size);
        bool resizeSwapChain(const Vec2& size);
        D3DDevice getD3DDevice();
        D3DDeviceContext getD3DDeviceContext();
        bool supportsR8G8B8A8_SNORM() const;
        bool supportsR16G16_UNORM() const;
    };

    D3D11_COMPARISON_FUNC getComparisonFunc(ComparisonFunc comparisonFunc);
}
