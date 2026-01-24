#include <typeinfo>
#include "BufferD3D11.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

using namespace mce;

DXGI_FORMAT DXGIFormatFromStride(unsigned int stride)
{
    switch (stride)
    {
    case 1:  return DXGI_FORMAT_R8_UINT;
    case 2:  return DXGI_FORMAT_R16_UINT;
    case 4:  return DXGI_FORMAT_R32_UINT;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}

D3D11_MAP mapTypeToD3D11MapType(MapType mapType)
{
    switch (mapType)
    {
    case MAP_READ:               return D3D11_MAP_READ;
    case MAP_WRITE:              return D3D11_MAP_WRITE;
    case MAP_WRITE_DISCARD:      return D3D11_MAP_WRITE_DISCARD;
    case MAP_WRITE_NO_OVERWRITE: return D3D11_MAP_WRITE_NO_OVERWRITE;
    default:
        LOG_E("Unknown mapType: %d", mapType);
        throw std::bad_cast();
    }
}

BufferD3D11::BufferD3D11()
{
    m_format = DXGI_FORMAT_UNKNOWN;
}

BufferD3D11::~BufferD3D11()
{
    releaseBuffer();
}

void BufferD3D11::_createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType, bool isDynamic)
{
    if (m_buffer)
    {
        LOG_E("Make sure to release the m_buffer first");
        throw std::bad_cast();
    }

    UINT bindFlags = 0x0;
    switch (bufferType)
    {
    case BUFFER_TYPE_VERTEX:
        bindFlags = D3D11_BIND_VERTEX_BUFFER;
        break;
    case BUFFER_TYPE_INDEX:
        bindFlags = D3D11_BIND_INDEX_BUFFER;
        m_format = DXGIFormatFromStride(stride);
        break;
    default:
        LOG_E("Unknown bufferType: %d", bufferType);
        throw std::bad_cast();
    }

    D3D11_BUFFER_DESC bufferDesc;
    {
        bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = bindFlags;
        bufferDesc.ByteWidth = stride * count;
        bufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0x0;
        bufferDesc.MiscFlags = 0x0;
        bufferDesc.StructureByteStride = 0;
    }

    D3D11_SUBRESOURCE_DATA subResourceData;
    {
        subResourceData.SysMemPitch = 0;
        subResourceData.SysMemSlicePitch = 0;
        subResourceData.pSysMem = data;
    }

    D3DDevice d3dDevice = context.getD3DDevice();
    if (data)
    {
        HRESULT hResult = d3dDevice->CreateBuffer(&bufferDesc, &subResourceData, *m_buffer);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
    else
    {
        HRESULT hResult = d3dDevice->CreateBuffer(&bufferDesc, NULL, *m_buffer);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }
}

void BufferD3D11::_move(BufferD3D11& other)
{
    if (this != &other)
    {
        this->releaseBuffer();
		
        this->m_buffer._move(other.m_buffer);
        std::swap(this->m_format, other.m_format);
        std::swap(this->m_bufferOffset, other.m_bufferOffset);
    }
	
    BufferBase::_move(other);
}

void BufferD3D11::releaseBuffer()
{
    m_buffer.release();

    BufferBase::releaseBuffer();
}

void BufferD3D11::bindBuffer(RenderContext& context)
{
    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();

    switch (m_bufferType)
    {
    case BUFFER_TYPE_VERTEX:
        d3dDeviceContext->IASetVertexBuffers(0, 1, *m_buffer, &m_stride, &m_bufferOffset);
        break;
    case BUFFER_TYPE_INDEX:
        d3dDeviceContext->IASetIndexBuffer(**m_buffer, m_format, m_bufferOffset);
        break;
    default:
        LOG_E("Unknown m_bufferType: %d", m_bufferType);
        throw std::bad_cast();
    }
}

void BufferD3D11::createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType)
{
    _createBuffer(context, stride, data, count, bufferType, false);
    BufferBase::createBuffer(context, stride, data, count, bufferType);
}

void BufferD3D11::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    _createBuffer(context, stride, data, count, bufferType, true);
    BufferBase::createDynamicBuffer(context, stride, data, count, bufferType);
}

void BufferD3D11::resizeBuffer(RenderContext& context, const void* data, unsigned int size)
{
    LOG_E("BufferD3D11::resizeBuffer() not implemented");
    throw std::bad_cast();
}

void BufferD3D11::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count, MapType mapType)
{
    D3D11_MAPPED_SUBRESOURCE subResource;
    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();

    if (m_bufferType == BUFFER_TYPE_INDEX)
    {
        m_format = DXGIFormatFromStride(stride);
    }

    {
        D3D11_MAP d3dMap = mapTypeToD3D11MapType(mapType);
        HRESULT hResult = d3dDeviceContext->Map(**m_buffer, 0, d3dMap, 0x0, &subResource);
        ErrorHandlerDXGI::checkForErrors(hResult);
    }

    if (subResource.RowPitch < stride * count)
    {
        d3dDeviceContext->Unmap(**m_buffer, 0);
        m_buffer.release();
        // Mojang did "stride * count" for the stride argument here, probably a bug
        createDynamicBuffer(context, stride, data, count, m_bufferType);
    }
    else
    {
        memcpy((int8_t*)subResource.pData + m_bufferOffset, data, stride * count);
        d3dDeviceContext->Unmap(**m_buffer, 0);
    }
        
    BufferBase::updateBuffer(context, stride, data, count);
}
