#pragma once

#include <vector>
#include <map>

#include "API_D3D9.hpp"
#include "renderer/hal/base/RenderContextBase.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"
#include "world/phys/Vec2.hpp"

namespace mce
{
    const D3DPRIMITIVETYPE modeMap[] = {
        /*PRIMITIVE_MODE_NONE*/           (D3DPRIMITIVETYPE)0,
        /*PRIMITIVE_MODE_QUAD_LIST*/      D3DPT_TRIANGLELIST,
        /*PRIMITIVE_MODE_TRIANGLE_LIST*/  D3DPT_TRIANGLELIST,
        /*PRIMITIVE_MODE_TRIANGLE_STRIP*/ D3DPT_TRIANGLESTRIP,
        /*PRIMITIVE_MODE_LINE_LIST*/      D3DPT_LINELIST,
        /*PRIMITIVE_MODE_LINE_STRIP*/     D3DPT_LINESTRIP
    };

    typedef ComInterface<IDirect3DDevice9> D3DDevice;

    class DepthStencilState;
    class RenderContextD3D9 : public RenderContextBase
    {
    public:
        struct VertexDeclID
        {
            VertexFormat vertexFormat;
            unsigned int attributeListIndex;

            VertexDeclID(const VertexFormat& vertexFormat, unsigned int attributeListIndex);

            bool operator<(const VertexDeclID& other) const;
            bool operator==(const VertexDeclID& other) const;
        };
        typedef std::map<VertexDeclID, ComInterface<IDirect3DVertexDeclaration9>> VertexDeclCache;

    private:
        IDirect3D9* m_pD3D;
        D3DDevice m_d3dDevice;

    public:
        D3DVIEWPORT9 m_viewport;
        unsigned int m_width;
        unsigned int m_height;
        VertexDeclCache m_vertexDeclCache;
        //IDirect3DConstantBuffer9* m_constantBuffers[SHADER_TYPES_COUNT][60];

    public:
        RenderContextD3D9();

    public:
        void draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize);
        void drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize);
        void setDepthRange(float nearVal, float farVal);
        void setViewport(unsigned int width, unsigned int height, float nearVal, float farVal, const ViewportOrigin& origin);
        void clearFrameBuffer(const Color& color);
        void clearStencilBuffer();
        void clearDepthStencilBuffer();
        void clearContextState();
        void beginRender();
        void endRender();
        void swapBuffers();

        bool supports8BitIndices() const;

        void createDeviceResources();
        void createWindowSizeDependentResources(HWND hWnd, unsigned int width, unsigned int height);
        D3DDevice getD3DDevice();
    };

    D3DCMPFUNC getComparisonFunc(ComparisonFunc comparisonFunc);
}
