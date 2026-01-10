#pragma once

#define WIN32_LEAN_AND_MEAN

// Must always include Winsock2.h before windows.h
#include <winsock2.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>


// https://github.com/microsoft/DirectXTK/wiki/DirectXHelpers
// "The MinGW compiler Direct3D 11 headers are missing CD3D11_RECT, CD3D11_VIEWPORT, and CD3D11_DEPTH_STENCIL_VIEW_DESC along with D3D11_MIN_DEPTH and D3D11_MAX_DEPTH."
#ifdef __MINGW32__
#define D3D11_MIN_DEPTH 0.0f
#define D3D11_MAX_DEPTH 1.0f

struct CD3D11_DEPTH_STENCIL_VIEW_DESC : public D3D11_DEPTH_STENCIL_VIEW_DESC
{
    CD3D11_DEPTH_STENCIL_VIEW_DESC() = default;
    explicit CD3D11_DEPTH_STENCIL_VIEW_DESC( const D3D11_DEPTH_STENCIL_VIEW_DESC& o ) :
        D3D11_DEPTH_STENCIL_VIEW_DESC( o )
    {}
    explicit CD3D11_DEPTH_STENCIL_VIEW_DESC(
        D3D11_DSV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        UINT mipSlice = 0,
        UINT firstArraySlice = 0,
        UINT arraySize = -1,
        UINT flags = 0 )
    {
        Format = format;
        ViewDimension = viewDimension;
        Flags = flags;
        switch (viewDimension)
        {
        case D3D11_DSV_DIMENSION_TEXTURE1D:
            Texture1D.MipSlice = mipSlice;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
            Texture1DArray.MipSlice = mipSlice;
            Texture1DArray.FirstArraySlice = firstArraySlice;
            Texture1DArray.ArraySize = arraySize;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2D:
            Texture2D.MipSlice = mipSlice;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
            Texture2DArray.MipSlice = mipSlice;
            Texture2DArray.FirstArraySlice = firstArraySlice;
            Texture2DArray.ArraySize = arraySize;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMS:
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
            Texture2DMSArray.FirstArraySlice = firstArraySlice;
            Texture2DMSArray.ArraySize = arraySize;
            break;
        default: break;
        }
    }
    explicit CD3D11_DEPTH_STENCIL_VIEW_DESC(
        _In_ ID3D11Texture1D* pTex1D,
        D3D11_DSV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        UINT mipSlice = 0,
        UINT firstArraySlice = 0,
        UINT arraySize = -1,
        UINT flags = 0 )
    {
        ViewDimension = viewDimension;
        Flags = flags;
        if (DXGI_FORMAT_UNKNOWN == format ||
            ((UINT)-1 == arraySize && D3D11_DSV_DIMENSION_TEXTURE1DARRAY == viewDimension))
        {
            D3D11_TEXTURE1D_DESC TexDesc;
            pTex1D->GetDesc( &TexDesc );
            if (DXGI_FORMAT_UNKNOWN == format) format = TexDesc.Format;
            if ((UINT)-1 == arraySize) arraySize = TexDesc.ArraySize - firstArraySlice;
        }
        Format = format;
        switch (viewDimension)
        {
        case D3D11_DSV_DIMENSION_TEXTURE1D:
            Texture1D.MipSlice = mipSlice;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
            Texture1DArray.MipSlice = mipSlice;
            Texture1DArray.FirstArraySlice = firstArraySlice;
            Texture1DArray.ArraySize = arraySize;
            break;
        default: break;
        }
    }
    explicit CD3D11_DEPTH_STENCIL_VIEW_DESC(
        _In_ ID3D11Texture2D* pTex2D,
        D3D11_DSV_DIMENSION viewDimension,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
        UINT mipSlice = 0,
        UINT firstArraySlice = 0,
        UINT arraySize = -1,
        UINT flags = 0 )
    {
        ViewDimension = viewDimension;
        Flags = flags;
        if (DXGI_FORMAT_UNKNOWN == format || 
            ((UINT)-1 == arraySize &&
                (D3D11_DSV_DIMENSION_TEXTURE2DARRAY == viewDimension ||
                D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY == viewDimension)))
        {
            D3D11_TEXTURE2D_DESC TexDesc;
            pTex2D->GetDesc( &TexDesc );
            if (DXGI_FORMAT_UNKNOWN == format) format = TexDesc.Format;
            if ((UINT)-1 == arraySize) arraySize = TexDesc.ArraySize - firstArraySlice;
        }
        Format = format;
        switch (viewDimension)
        {
        case D3D11_DSV_DIMENSION_TEXTURE2D:
            Texture2D.MipSlice = mipSlice;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
            Texture2DArray.MipSlice = mipSlice;
            Texture2DArray.FirstArraySlice = firstArraySlice;
            Texture2DArray.ArraySize = arraySize;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMS:
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
            Texture2DMSArray.FirstArraySlice = firstArraySlice;
            Texture2DMSArray.ArraySize = arraySize;
            break;
        default: break;
        }
    }
    ~CD3D11_DEPTH_STENCIL_VIEW_DESC() {}
};

#endif // defined(__MINGW32__)
